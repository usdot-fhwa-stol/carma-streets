#include "signalized_vehicle_scheduler.h"

namespace streets_vehicle_scheduler {

    void signalized_vehicle_scheduler::schedule_vehicles( std::unordered_map<std::string, streets_vehicles::vehicle> &vehicles, std::shared_ptr<intersection_schedule> &i_sched) {
        
        try {
            auto schedule = std::dynamic_pointer_cast<signalized_intersection_schedule> (i_sched);
            
            if ( vehicles.empty() ) {
                SPDLOG_DEBUG("No vehicles to schedule.");
                return;
            }

            // Estimate Vehicles at common time 
            estimate_vehicles_at_common_time( vehicles, schedule->timestamp);
            // Create vectors of DVs and EVs
            std::list<streets_vehicles::vehicle> DVs;
            std::list<streets_vehicles::vehicle> EVs;
            
            for ( const auto &[v_id, veh] : vehicles) {
                if ( veh._cur_state == streets_vehicles::vehicle_state::EV) {
                    EVs.push_back(veh);
                }
                else if ( veh._cur_state == streets_vehicles::vehicle_state::DV ) {
                    DVs.push_back(veh);
                }
            }
            
            SPDLOG_DEBUG("Number of Entering Vehicles (EVs) to schedule are : {0} ", EVs.size());

            // Schedule DVs
            if ( !DVs.empty() )
                schedule_dvs( DVs, schedule);
            // Schedule EVs
            if ( !EVs.empty() )
                schedule_evs( EVs, schedule);
        }
        catch ( const streets_service::streets_configuration_exception &ex ) {
            SPDLOG_ERROR("signalized scheduler failure: {0} ", ex.what());
        }

    }


    void signalized_vehicle_scheduler::schedule_dvs( const std::list<streets_vehicles::vehicle> &dvs, const std::shared_ptr<signalized_intersection_schedule> &schedule ) const {
        
        for ( const auto &departing_veh : dvs ) {
            SPDLOG_DEBUG("Scheduling the departure time for DV with ID {0} .", departing_veh._id);
            // get link lane info
            OpenAPI::OAILanelet_info lane_info =  get_link_lanelet_info( departing_veh );
            // calculate clearance time in milliseconds 
            uint64_t clearance_time = estimate_clearance_time( departing_veh );

            signalized_vehicle_schedule veh_sched;
            // set id
            veh_sched.v_id = departing_veh._id;
            // set et 
            veh_sched.et = departing_veh._actual_et;
            // departure time is equal to current time plus clearance time for DVs
            veh_sched.dt =  schedule->timestamp + clearance_time;
            // set state
            veh_sched.state = departing_veh._cur_state;
            // set vehicle link id
            veh_sched.link_id =  departing_veh._link_id;
            // set vehicle entry lane id
            veh_sched.entry_lane =  departing_veh._entry_lane_id;

            SPDLOG_DEBUG("Added schedule for DV {0} with dt {1}.", veh_sched.v_id, veh_sched.dt);
            // add vehicle_schedule
            schedule->vehicle_schedules.push_back(veh_sched);
        }

    }



    void signalized_vehicle_scheduler::schedule_evs( std::list<streets_vehicles::vehicle> &evs, const std::shared_ptr<signalized_intersection_schedule> &schedule ) const {
        
        // Sort vehicles based on distance
        evs.sort(distance_comparator);
        
        // Create a map of entry lane id keys and list of EVs in each lane.
        std::unordered_map<int, std::list<streets_vehicles::vehicle>> vehicle_lane_map;
        for ( const auto &entry_lane : intersection_info->getEntryLanelets() ) {
            std::list<streets_vehicles::vehicle> vehicles_in_lane;
            for ( const auto &ev : evs ) {
                if ( ev._entry_lane_id == entry_lane.getId()) {
                    SPDLOG_DEBUG("Adding vehicle {0} to EVs list in entry lane {1}", ev._id, ev._entry_lane_id);
                    vehicles_in_lane.push_back(ev);
                }
            }
            if ( !vehicles_in_lane.empty()) {
                vehicles_in_lane.sort(distance_comparator);
                vehicle_lane_map.try_emplace( entry_lane.getId(), vehicles_in_lane );
            }
            else {
                SPDLOG_DEBUG("No EVs in lane {0}.", static_cast<int>(entry_lane.getId()) );
            }
        }
        if ( vehicle_lane_map.empty() ) {
            throw scheduling_exception("Map of vehicles to be scheduled is empty but list of EVs to be scheduled is not!");
        }
        
        for ( const auto &[entry_lane, evs_in_lane] : vehicle_lane_map ) {           
            
            SPDLOG_DEBUG("Scheduling EVs from entry lane {0} ", entry_lane);

            // Find the last DV from the entry lane
            std::shared_ptr<signalized_vehicle_schedule> preceding_veh = nullptr;
            for (const auto &veh_sched : schedule->vehicle_schedules ) {
                if ( veh_sched.entry_lane == entry_lane && veh_sched.state == streets_vehicles::vehicle_state::DV && (preceding_veh == nullptr || veh_sched.et < preceding_veh->et)) {
                    preceding_veh = std::make_shared<signalized_vehicle_schedule>(veh_sched);
                }
            }
            
            // Find the entry lane object
            OpenAPI::OAILanelet_info entry_lane_info;
            for ( const auto &lane : intersection_info->getEntryLanelets() ){
                if ( lane.getId() == entry_lane ) {
                    entry_lane_info = lane;
                    break;
                }
            }
            SPDLOG_DEBUG("The signal group id of the entry lane {0} = {1}", entry_lane_info.getId(), entry_lane_info.getSignalGroupId());

            // Get the movement_state object that connects to this entry lane
            signal_phase_and_timing::movement_state move_state = find_movement_state_for_lane(entry_lane_info);

            for (const auto &ev : evs_in_lane){
                signalized_vehicle_schedule sched;
                SPDLOG_DEBUG( "Estimating schedule for {0}.", ev._id);
                estimate_et(ev, preceding_veh, sched, move_state, schedule->timestamp);
                // Add vehicle schedule to the schedule list.
                schedule->vehicle_schedules.push_back(sched);
                // Update the preceding vehicle schedule.
                preceding_veh = std::make_shared<signalized_vehicle_schedule>(sched);
            }

            SPDLOG_DEBUG("All vehicles in lane {0} have been scheduled!", entry_lane);

        }
    }


    signal_phase_and_timing::movement_state signalized_vehicle_scheduler::find_movement_state_for_lane(const OpenAPI::OAILanelet_info &entry_lane_info) const {

        signal_phase_and_timing::movement_state move_state; 
        if ( spat_ptr || entry_lane_info.getSignalGroupId() ) {
            for (const auto& ms : spat_ptr->intersections.front().states){
                if (ms.signal_group == entry_lane_info.getSignalGroupId()) {
                    move_state = ms;
                    return move_state;
                }
            }
        }
        else {
            if ( spat_ptr ) {
                throw scheduling_exception("SPaT is not found!");
            }
            else {
                throw scheduling_exception("the lanelet does not have group_id!");
            }
        }
        return move_state;
    }


    void signalized_vehicle_scheduler::estimate_et(const streets_vehicles::vehicle &veh, const std::shared_ptr<signalized_vehicle_schedule> &preceding_veh, signalized_vehicle_schedule &sched, const signal_phase_and_timing::movement_state &move_state, const uint64_t schedule_timestamp) const {

        // Get link lanelet information for ev
        OpenAPI::OAILanelet_info link_lane = get_link_lanelet_info( veh );
        SPDLOG_DEBUG( "Link lanelet for vehicle {0} is {1}.", veh._id, link_lane.getId());
        // Calculate EET for vehicle
        uint64_t eet = calculate_earliest_entering_time(veh);
        SPDLOG_DEBUG( "EET for vehicle {0} is {1}." ,veh._id, eet );
        // Calculate min_headway
        uint64_t min_headway = calculate_min_headway( veh, link_lane.getSpeedLimit() );
        SPDLOG_DEBUG( "min headway for vehicle {0} is {1}.", veh._id, min_headway );
        /** estimate the earliest possible ET based on the current timestamp, the preceding vehicle's estimated ET, 
         * and the subject vehicle's minimum required safety time headway at tbe departure speed.
        */
        uint64_t first_available_et;
        if (preceding_veh == nullptr) {
            first_available_et = schedule_timestamp;
        }
        else {
            first_available_et = std::max(schedule_timestamp, preceding_veh->et + min_headway);
        }

        // Check the movement_event list one by one until successfully estimating the vehicle's entering time (ET).
        // The assumption is that the movement_event list is sorted based on the movement_event timing.
        // (example: the second phase's start time shall be equal to the first phase's min_end_time).
        bool is_successful = false;
        uint64_t et;
        for (const auto &move_event : move_state.state_time_speed) {
            SPDLOG_DEBUG("Moving to the next movement event from the list! start time without buffer = {0}, end time without buffer = {1}", move_event.timing.get_epoch_start_time(), move_event.timing.get_epoch_min_end_time() );
            if (move_event.event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed) {
                et = std::max(first_available_et, std::max(eet, move_event.timing.get_epoch_start_time() + initial_green_buffer));
                if ( et < move_event.timing.get_epoch_min_end_time() - final_green_buffer ) {
                    SPDLOG_DEBUG( "Successfully estimate an ET (within a green phase) for vehicle {0}. The estimated ET = {1}.", veh._id, et);
                    is_successful = true;
                    break;
                }
                SPDLOG_DEBUG( "The estimated ET for vehicle {0} is later than the end of the phase. Estimated ET = {1}, end of the phase", veh._id, et, move_event.timing.get_epoch_min_end_time() - final_green_buffer );
            }
        }

        // TBD area
        if (!is_successful) {
            et = std::max(first_available_et, std::max(eet, move_state.state_time_speed.back().timing.get_epoch_min_end_time() + initial_green_buffer));
            SPDLOG_DEBUG( "Successfully estimate an ET (within TBD area) for vehicle {0}. The estimated ET = {1}.", veh._id, et);
        }

        // Set schedule properties
        sched.v_id =  veh._id;
        sched.eet = eet;
        sched.et = et;
        sched.dt = sched.et + estimate_clearance_time( veh );
        sched.entry_lane =  veh._entry_lane_id;
        sched.link_id = veh._link_id;
        sched.state = streets_vehicles::vehicle_state::EV;
    }


    uint64_t signalized_vehicle_scheduler::calculate_earliest_entering_time(const streets_vehicles::vehicle &veh) const {
        
        if ( veh._cur_state == streets_vehicles::vehicle_state::EV) {
            // Distance to stop bar 
            double delta_x = veh._cur_distance;
            // Get Entry Lane
            OpenAPI::OAILanelet_info entry_lane =  get_entry_lanelet_info( veh );
            // Get Link Lane
            OpenAPI::OAILanelet_info link_lane =  get_link_lanelet_info( veh );
            // Distance necessary to get to max speed and decelerate with decel_max to departure speed
            double delta_x_prime =  calculate_distance_accel_and_decel( veh, entry_lane.getSpeedLimit(), link_lane.getSpeedLimit() );
            // Distance necessary to get to the departure speed
            double delta_x_zegond =  calculate_distance_accel_or_decel( veh, link_lane.getSpeedLimit() );
            SPDLOG_DEBUG("Delta X = {0}, Delta X Prime = {1}, Delta X Zegond = {2}.", delta_x, delta_x_prime, delta_x_zegond);

            // Calculate v_hat
            double v_hat = calculate_v_hat(veh, entry_lane.getSpeedLimit(), link_lane.getSpeedLimit(), delta_x, delta_x_prime, delta_x_zegond);
            SPDLOG_DEBUG("V hat = {0}.", v_hat);

            // calculate planned acceleration time interval
            double t_accel = calculate_acceleration_time(veh, v_hat, link_lane.getSpeedLimit(), delta_x, delta_x_zegond);
            SPDLOG_DEBUG("T accel = {0}.",t_accel);

            // calculate planned deceleration time interval
            double t_decel = calculate_deceleration_time(veh, v_hat, link_lane.getSpeedLimit(), delta_x, delta_x_zegond);
            SPDLOG_DEBUG("T decel = {0}.",t_decel);

            // Calculate planned cruising time interval
            double t_cruising = calculate_cruising_time(v_hat, delta_x, delta_x_prime);
            SPDLOG_DEBUG("T cruising = {0}.",t_cruising);
            
            // calculate earliest entering time to stop bar
            double eet = t_accel + t_cruising + t_decel;
            if (eet <= 0) {
                SPDLOG_DEBUG("The estimated eet for vehicle {0} has a negative value = {1}!", veh._id, eet);
            }
            return static_cast<uint64_t>(ceil(eet * 1000.0)) + veh._cur_time;
        }
        else
        {
            SPDLOG_DEBUG("Cannot estimate the earliest entering time for vehicle {0} which is not an EV!", veh._id);
            throw scheduling_exception("Trying to calculate earliest entering time (EET) for a vehicle that is not an EV!");
        }
        return -1;
    }


    double signalized_vehicle_scheduler::calculate_distance_accel_and_decel( const streets_vehicles::vehicle &veh, const double max_speed, const double departure_speed) const {
        // calculate delta x prime
        double delta_x_prime = ((pow(max_speed, 2) - pow(veh._cur_speed, 2)) / (2 * veh._accel_max)) + ((pow(departure_speed, 2) - pow(max_speed, 2)) / (2 * veh._decel_max));
        return  delta_x_prime;
    }


    double signalized_vehicle_scheduler::calculate_distance_accel_or_decel( const streets_vehicles::vehicle &veh, const double departure_speed ) const {
        // calculate delta x zegond
        double delta_x_zegond;
        if (veh._cur_speed <= departure_speed) {
            delta_x_zegond = (pow(departure_speed, 2) - pow(veh._cur_speed, 2)) / (2 * veh._accel_max);
        }
        else {
            delta_x_zegond = (pow(departure_speed, 2) - pow(veh._cur_speed, 2)) / (2 * veh._decel_max);
        }
        return  delta_x_zegond;
    }


    double signalized_vehicle_scheduler::calculate_v_hat( const streets_vehicles::vehicle &veh, const double max_speed, const double departure_speed, const double delta_x, const double delta_x_prime, const double delta_x_zegond ) const {
        if (delta_x >= delta_x_prime) {
            return max_speed;
        }
        else if (delta_x_prime > delta_x && delta_x >= delta_x_zegond) {
            double numerator = (2 * delta_x * veh._decel_max * veh._accel_max) + (veh._decel_max * pow(veh._cur_speed, 2)) - (veh._accel_max * pow(departure_speed, 2));
            double denominator = veh._decel_max - veh._accel_max;
            return sqrt(numerator/denominator);
        }
        else {
            if (veh._cur_speed <= departure_speed) {
                return sqrt((2 * delta_x * veh._accel_max) + pow(veh._cur_speed, 2));
            }
            else {
                return sqrt((2 * delta_x * veh._decel_max) + pow(veh._cur_speed, 2));
            }
        }
    }


    double signalized_vehicle_scheduler::calculate_acceleration_time(const streets_vehicles::vehicle &veh, const double v_hat, const double departure_speed, const double delta_x, const double delta_x_zegond) const {
        double t_accel = 0.0;
        if (delta_x >= delta_x_zegond || departure_speed >= veh._cur_speed){
            if (v_hat >= veh._cur_speed) {
                t_accel = (v_hat - veh._cur_speed) / veh._accel_max;
            }
            else {
                SPDLOG_DEBUG("Negative acceleration time interval is calculated for vehicle {0}. The acceleration time interval is set to 0.", veh._id);
            }
        }
        return t_accel;
    }


    double signalized_vehicle_scheduler::calculate_deceleration_time(const streets_vehicles::vehicle &veh, const double v_hat, const double departure_speed, const double delta_x, const double delta_x_zegond) const {
        double t_decel = 0.0;
        if (delta_x < delta_x_zegond && departure_speed < veh._cur_speed) {
            if (v_hat <= veh._cur_speed) {
                t_decel = (v_hat - veh._cur_speed) / veh._decel_max;
            }
            else {
                SPDLOG_DEBUG("Negative deceleration time interval is calculated for vehicle {0}. The deceleration time interval is set to 0.", veh._id);
            }
        }
        else if (delta_x >= delta_x_zegond) {
            if (v_hat >= departure_speed) {
                t_decel = (departure_speed - v_hat) / veh._decel_max;
            }
            else {
                SPDLOG_DEBUG("Negative deceleration time interval is calculated for vehicle {0}. The deceleration time interval is set to 0.", veh._id);
            }
        }
        return t_decel;
    }


    double signalized_vehicle_scheduler::calculate_cruising_time(const double v_hat, const double delta_x, const double delta_x_prime) const {
        if (delta_x > delta_x_prime) {
            return (delta_x - delta_x_prime) / v_hat;
        }
        return 0.0;
    }


    uint64_t signalized_vehicle_scheduler::calculate_min_headway(const streets_vehicles::vehicle &veh, const double speed) const {
        
        return static_cast<uint64_t>(ceil( 1000*(veh._reaction_time + ((veh._length + veh._min_gap)/speed)) ));
    } 


    uint64_t signalized_vehicle_scheduler::estimate_clearance_time( const streets_vehicles::vehicle &veh ) const {
        // Get Link Lane
        OpenAPI::OAILanelet_info link_lane =  get_link_lanelet_info( veh );
        if ( veh._cur_state == streets_vehicles::vehicle_state::DV ) {
            return static_cast<uint64_t>( ceil(1000 * veh._cur_distance / veh._cur_speed) );
        }
        else {
            return static_cast<uint64_t>( ceil(1000 * link_lane.getLength() / link_lane.getSpeedLimit()) );
        }
    }


    void signalized_vehicle_scheduler::set_initial_green_buffer(const uint64_t buffer){
        initial_green_buffer = buffer;
    }

    uint64_t signalized_vehicle_scheduler::get_initial_green_buffer() const {
        return initial_green_buffer;
    }

    void signalized_vehicle_scheduler::set_final_green_buffer(const uint64_t buffer){
        final_green_buffer = buffer;
    }

    uint64_t signalized_vehicle_scheduler::get_final_green_buffer() const {
        return final_green_buffer;
    }

    std::shared_ptr<signal_phase_and_timing::spat> signalized_vehicle_scheduler::get_spat() const {
        return spat_ptr;
    }

    void signalized_vehicle_scheduler::set_spat(std::shared_ptr<signal_phase_and_timing::spat> spat_info) {
        spat_ptr = spat_info;
    }
    
}