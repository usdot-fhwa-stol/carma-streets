#include "all_stop_vehicle_scheduler.h"

namespace streets_vehicle_scheduler {

   

    void all_stop_vehicle_scheduler::schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                            intersection_schedule &schedule) {
        if ( vehicles.empty() ) {
            SPDLOG_INFO("No vehicles to schedule.");
            return;
        }
        // Estimate Vehicles at common time 
        estimate_vehicles_at_common_time( vehicles, schedule.timestamp);
        // Create vectors of EVs RDVs and DVs
        std::list<streets_vehicles::vehicle> DVs;
        std::list<streets_vehicles::vehicle> RDVs;
        std::list<streets_vehicles::vehicle> RDVs_with_access;

        std::list<streets_vehicles::vehicle> EVs;
        for ( const auto &[v_id, veh] : vehicles) {
            if ( veh._cur_state == streets_vehicles::vehicle_state::DV ) {
                // Once vehicle is considered as DV it has confirmed that it has received access.
                if ( is_rdv_previously_granted_access(veh) ) {
                    remove_rdv_previously_granted_access(veh);
                }
                DVs.push_back(veh);
            }
            else if ( veh._cur_state == streets_vehicles::vehicle_state::RDV ) {
                // Consider any RDVs previously granted access as a DV
                if ( is_rdv_previously_granted_access(veh) ) {
                    DVs.push_back(veh);
                } 
                else {
                    RDVs.push_back(veh);
                }
            }
            else if ( veh._cur_state == streets_vehicles::vehicle_state::EV) {
                EVs.push_back(veh);
            }
        }
        
        SPDLOG_DEBUG("Vehicles to schedule are DVs : {0}, RDVs : {1}, EVs {2} ", DVs.size(), RDVs.size(), EVs.size());

        // Schedule DVs
        if ( !DVs.empty() )
            schedule_dvs( DVs, schedule);
        // Schedule RDVs
        if ( !RDVs.empty() )
            schedule_rdvs( RDVs, schedule);
        // Schedule EVs
        if ( !EVs.empty() )
            schedule_evs( EVs, schedule);
        
        

    }

    double all_stop_vehicle_scheduler::estimate_delta_x_prime(const streets_vehicles::vehicle &veh, 
                                                                const OpenAPI::OAILanelet_info &entry_lane) const{
        if ( veh._cur_state == streets_vehicles::vehicle_state::EV ) {
            // Get Entry Lanelet
            double delta_x_prime = (pow(entry_lane.getSpeedLimit(),2)-pow(veh._cur_speed,2))/(2*veh._accel_max) - 
                pow(entry_lane.getSpeedLimit(), 2)/(2*veh._decel_max); 
            return  delta_x_prime;
        }
        else {
            SPDLOG_WARN("Cannot estimate delta x prime for vehicle {0} because it's state {1} != EV !" );
        }
    }

    double all_stop_vehicle_scheduler::calculate_v_hat(const streets_vehicles::vehicle &veh) const {
        double numerator = veh._decel_max *( 2 * veh._cur_distance*veh._accel_max + pow(veh._cur_speed, 2));
        double denominator = veh._decel_max-veh._accel_max;
        double v_hat = sqrt(numerator/denominator);
        // If v_hat is less that current vehicle speed, const maximum deceleration will not stop before stop bar
        if ( v_hat < veh._cur_speed) {
            SPDLOG_ERROR("Stopping trajectory not possible for vehicle {0} with speed {1} m/s and v_hat {2} m/s!", veh._id, veh._cur_speed, v_hat);
        }
        return v_hat;
    }

    double all_stop_vehicle_scheduler::calculate_acceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const {
        // If v_hat is less than current vehicle speed there is only time for deceleration
        // Note: This is an error case and means stopping before the stop bar is not possible given current kinematic information
        // and deceleration limits
        if ( v_hat < veh._cur_speed ){
            return 0.0;
        } 
        return (v_hat-veh._cur_speed)/veh._accel_max;
    }

    double all_stop_vehicle_scheduler::calculate_cruising_time( const streets_vehicles::vehicle &veh, const double v_hat, const double delta_x_prime) const {
        // If v_hat is less than current speed there is only time for deceleration
        // Note: This is an error case and means stopping before the stop bar is not possible given current kinematic information
        // and deceleration limits
        if ( v_hat < veh._cur_speed ) {
            return 0.0;
        }
        return (veh._cur_distance - delta_x_prime)/v_hat; 
    }

    double all_stop_vehicle_scheduler::calculate_deceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const {
        return -v_hat/veh._decel_max;
    }

    void all_stop_vehicle_scheduler::schedule_dvs( std::list<streets_vehicles::vehicle> &dvs,
                                                intersection_schedule &schedule) const{
        // Sort based on departure position
        dvs.sort( departure_position_comparator);                                                    
        for ( const auto &departing_veh : dvs ) {
            SPDLOG_DEBUG("Scheduling DV with ID {0} .", departing_veh._id);
            // get link lane info
            OpenAPI::OAILanelet_info lane_info =  get_link_lanelet_info( departing_veh );
            // calculate clearance time in milliseconds 
            uint64_t clearance_time = estimate_clearance_time(departing_veh, lane_info);

            vehicle_schedule veh_sched;
            // set id
            veh_sched.v_id = departing_veh._id;
            // set est 
            veh_sched.est = departing_veh._actual_st;
            // set st
            veh_sched.st = departing_veh._actual_st;
            // set et
            veh_sched.et =  departing_veh._actual_et;
            // departure time is equal to current time plus clearance time for DVs
            veh_sched.dt =  schedule.timestamp + clearance_time;
            // set departure position
            veh_sched.dp = departing_veh._departure_position;
            // set state
            veh_sched.state = departing_veh._cur_state;
            // set vehicle link id
            veh_sched.link_id =  departing_veh._link_id;
            // set vehicle entry lane id
            veh_sched.entry_lane =  departing_veh._entry_lane_id;
            // set access
            veh_sched.access =  true;
            SPDLOG_DEBUG("Added schedule for {0} with dt {1}.", veh_sched.v_id, veh_sched.dt);
            // add vehicle_schedule
            schedule.vehicle_schedules.push_back(veh_sched);

        }

    }

    void all_stop_vehicle_scheduler::schedule_rdvs( std::list<streets_vehicles::vehicle> &rdvs, intersection_schedule &schedule ) {
        // Vector of considered intersection_schedule_options
        std::vector<intersection_schedule> schedule_options;
        // Sort rdvs ascending order based on departure position
        rdvs.sort(departure_position_comparator);
        // Earliest possible departure position is equal to first RDVs departure position
        int starting_departure_position = rdvs.begin()->_departure_position;
        SPDLOG_DEBUG("Staring the schedule RDVs from departure index {0}!", starting_departure_position );
        // TODO: for 8 possible approaches this may need to be optimized (greedy search)
        do { 
            SPDLOG_DEBUG("Considering scheduling options with {0} as first RDV." ,rdvs.front()._id);
            // update schedule option to include already scheduled vehicles
            intersection_schedule option = schedule;
            // consider current permutation of RDV departure order
            bool valid_option =  consider_departure_position_permutation(rdvs, option, starting_departure_position);
            SPDLOG_DEBUG("Scheduling option is valid : {0}.", valid_option);
            // is permutation valid in terms of flexibility limit? Has any vehicle's departure index changed by more
            // than the current flexibility limit.
            if ( valid_option ) {
                schedule_options.push_back(option);
            }

        }
        while (std::next_permutation( rdvs.begin(), rdvs.end(), departure_position_comparator));
        // If no valid options were added.
        if ( schedule_options.empty()) {
            throw scheduling_exception("There are no valid schedules for RDVs! Please check flexibility_limit setting.");
        }
        // Sort in ascending order based on delay.
        std::sort(schedule_options.begin(), schedule_options.end(), delay_comparator);

        SPDLOG_DEBUG("All all scheduling options, the option with RDV {0} first has the least delay of {1}.",
                                schedule_options.front().vehicle_schedules[starting_departure_position-1].v_id,
                                schedule_options.front().get_delay());
       
        // Add scheduled RDVs starting at last scheduled DV position to schedule
        for ( const auto &sched : schedule_options.front().vehicle_schedules){
            // starting at first available departure position for dvs, add rdvs from best option to current schedule
            if (starting_departure_position <= sched.dp ) {
                // if rdv was granted access in this schedule add it to the list of RDVs granted access
                if ( sched.access ) {
                        streets_vehicles::vehicle rdv_granted_access = get_vehicle_with_id( rdvs, sched.v_id );
                        SPDLOG_DEBUG("Added RDV {0} to list of RDVs previously granted access.", rdv_granted_access._id);
                        rdvs_previously_granted_access.push_back(rdv_granted_access);
                }
                schedule.vehicle_schedules.push_back( sched );
                starting_departure_position ++;
            }
        }
        SPDLOG_DEBUG("Schedule for RDVs: \n"+schedule.toCSV());
    }

    void all_stop_vehicle_scheduler::schedule_evs( std::list<streets_vehicles::vehicle> &evs,intersection_schedule &schedule ) const {
        // Map of entry lane ids to preceding already scheduled vehicle
        std::unordered_map<int , vehicle_schedule> preceding_vehicle_entry_lane_map;
        for ( const auto &entry_lane : intersection_info->getEntryLanelets() ) {
            int lane_id = entry_lane.getId();
            vehicle_schedule preceding_veh;
            // Only one RDV can exist for each approach
            for (const auto &veh_sched : schedule.vehicle_schedules ) {
                if ( veh_sched.state == streets_vehicles::vehicle_state::RDV) {
                    // All RDVs
                    preceding_veh = veh_sched;
                    preceding_vehicle_entry_lane_map.try_emplace(lane_id, preceding_veh);  

                }
                // Consider vehicles recently granted access
                else if ( veh_sched.state == streets_vehicles::vehicle_state::DV && (veh_sched.et-schedule.timestamp) <= entering_time_buffer) {
                    SPDLOG_DEBUG("Considering DV {0} as still in entry since entering time {1} - timestamp {2} does not exceed buffer {3}",
                            veh_sched.v_id, veh_sched.et, schedule.timestamp, entering_time_buffer);
                    preceding_veh = veh_sched;
                    preceding_vehicle_entry_lane_map.try_emplace(lane_id, preceding_veh);
                }
            }
        }

        // Sort vehicles based on distance
        evs.sort(distance_comparator);
        
        // Create a map of entry lane id keys and list of vehicle to be scheduled next in each lane.
        std::unordered_map<int, std::list<streets_vehicles::vehicle>> vehicle_to_be_scheduled_next;
        for ( const auto &entry_lane : intersection_info->getEntryLanelets() ) {
            std::list<streets_vehicles::vehicle> vehicles_in_lane;
            for ( const auto &ev : evs ) {
                if ( ev._entry_lane_id == entry_lane.getId()) {
                    SPDLOG_DEBUG("Adding vehicle {0} to EVs list in entry lane {1}", ev._id, ev._entry_lane_id);
                    vehicles_in_lane.push_back(ev);
                }
            }
            if ( !vehicles_in_lane.empty())
                vehicle_to_be_scheduled_next.try_emplace( entry_lane.getId(), vehicles_in_lane );
            else {
                SPDLOG_DEBUG("No EVs in lane {0}.",static_cast<int>(entry_lane.getId()) );
            }
        }
        if ( !vehicle_to_be_scheduled_next.empty()) {
            for ( const auto &[entry_lane, next_veh] : vehicle_to_be_scheduled_next ) {
                SPDLOG_DEBUG("Next Vehicle {0} to be scheduled in lane {1}",next_veh.front()._id, entry_lane);
            }
        }
        else {
            throw scheduling_exception("Map of vehicles to be scheduled is empty but list of EVs to be scheduled is not!");
        }
        // Get next available departure index, default to 1 if schedule is empty
        int last_departure_index = 1;
        if ( !schedule.vehicle_schedules.empty()) {
            last_departure_index =  schedule.vehicle_schedules.back().dp + 1;
        }
        do {
            vehicle_schedule sched;
            // Calculate ST for each next vehicle in each lane and schedule the vehicle with the lowest ST
            uint64_t lowest_st = 0;

            //Populate Map
            // Find next vehicle for each entry lanelet
            for ( const auto &[entry_lane, evs_in_lane] : vehicle_to_be_scheduled_next ) {
                // Take first vehicle to be scheduled in lane.
                streets_vehicles::vehicle ev = evs_in_lane.front();
                SPDLOG_DEBUG( "Estimating schedule for {0}.", ev._id);

                // Get link lanelet information for ev
                OpenAPI::OAILanelet_info link_lane = get_link_lanelet_info( ev );
                SPDLOG_DEBUG( "Link lanelet for {0} is {1}.", ev._id, link_lane.getId());
                // Calculate EST for vehicle
                uint64_t est = estimate_earliest_time_to_stop_bar(ev);
                SPDLOG_DEBUG( "EST for vehicle {0} is {1}." ,ev._id, est ) ;
                // Store ST value for vehicle
                uint64_t st;


                // If there is a preceeding vehicle scheduled in this lane
                if ( preceding_vehicle_entry_lane_map.find( ev._cur_lane_id )  != preceding_vehicle_entry_lane_map.end() ) {
                    // Get preceeding vehicle in lane
                    vehicle_schedule preceding_veh = preceding_vehicle_entry_lane_map.find( ev._cur_lane_id )->second;
                    SPDLOG_DEBUG("Preceding vehicle schedule in lane {0} is {1}.", preceding_veh.entry_lane, preceding_veh.v_id);
                    // If there is a preceeding vehicle the st is calculated as max of EST and preceeding vehicle ET plus a time
                    // buffer to account for the time it takes the preceeding vehicle to enter the intersection
                    SPDLOG_DEBUG("Preceding vehicle et {0}", preceding_veh.et);
                    st =  std::max( est, preceding_veh.et + entering_time_buffer );
                    SPDLOG_DEBUG("Setting st for vehicle {0} to max of {1} , {2}.", ev._id, est, preceding_veh.et + entering_time_buffer);

                }
                // IF there is no preceeding vehicle ST == EST
                else {
                    SPDLOG_DEBUG("No proceeding vehicle for {0}", ev._id);
                    st = est;
                }
                // If lowest st has not been set yet. This is currently the lowest st
                if ( lowest_st == 0 || lowest_st > st ) {
                    lowest_st = st;
                    // Set schedule properties
                    sched.v_id =  ev._id;
                    sched.est = est;
                    sched.st = st;
                    sched.entry_lane =  ev._entry_lane_id;
                    sched.link_id = ev._link_id;
                    sched.state = streets_vehicles::vehicle_state::EV;
                    sched.access = false;
                    sched.dp = last_departure_index;

                    // Get schedule of conflicting vehicle with largest dt from already scheduled vehicles
                    if ( !schedule.vehicle_schedules.empty() ) {
                        std::shared_ptr<vehicle_schedule> conflict_with_largest_dt = get_latest_conflicting(link_lane,schedule.vehicle_schedules);
                        
                        if ( conflict_with_largest_dt != nullptr) {
                            SPDLOG_DEBUG("Vehicle with largest conflict to {0} is {1}.", ev._id, conflict_with_largest_dt->v_id);
                            // Entering time is the maximum between the conflicting vehicles departure time and the current vehicles stopping time
                            sched.et = std::max(conflict_with_largest_dt->dt, sched.st);
                        }else{
                            // If there is no conflicting scheduled vehicle, st == et
                            sched.et = sched.st;
                        }
                    }
                    else {
                        // If schedule is empty there can be no conflict
                        sched.et = sched.st;
                    }
                    // Departure time is equal to entering time + clearance time
                    sched.dt = sched.et + estimate_clearance_time( ev, link_lane );
            
                }
               
            }
            SPDLOG_DEBUG( "Found vehicle {0} with lowest stopping time {1} in lane {2}", sched.v_id, sched.st, sched.entry_lane);
            // Add lowest ST to schedule
            schedule.vehicle_schedules.push_back(sched);
            // Increment departure index
            last_departure_index++;
            // Replace previous preceeding vehicle for lane with scheduled vehicle
            if ( preceding_vehicle_entry_lane_map.find(sched.entry_lane) != preceding_vehicle_entry_lane_map.end()) {
                preceding_vehicle_entry_lane_map.find(sched.entry_lane)->second =  sched;
            }
            else {
                preceding_vehicle_entry_lane_map.try_emplace(sched.entry_lane, sched);
            }
            // Pop vehicle off of list of vehicles to be scheduled for each lane
            vehicle_to_be_scheduled_next.find(sched.entry_lane)->second.pop_front();
            // If pop makes list empty remove map entry
            if ( vehicle_to_be_scheduled_next.find(sched.entry_lane)->second.empty() ) {
                SPDLOG_DEBUG("All vehicles in approach lanelet {0} have been scheduled!", sched.entry_lane);
                vehicle_to_be_scheduled_next.erase(sched.entry_lane);
            }
            
        }
        while ( !vehicle_to_be_scheduled_next.empty() );
    }

    uint64_t all_stop_vehicle_scheduler::estimate_earliest_time_to_stop_bar(const streets_vehicles::vehicle &veh) const{
        // Distance to stop bar 
        double delta_x = veh._cur_distance;
        // Get Entry Lane
        OpenAPI::OAILanelet_info lane_info =  get_entry_lanelet_info( veh );
        // Distance necessary to get to max speed and decelerate with decel_max
        double delta_x_prime =  estimate_delta_x_prime( veh, lane_info );
        SPDLOG_DEBUG("Delta X Prime = {0}.", delta_x_prime);

        // Calculate v_hat and planned cruising time interval
        double v_hat;
        double t_cruising;
        if ( delta_x >= delta_x_prime ) {
            v_hat = lane_info.getSpeedLimit();
            SPDLOG_DEBUG("V hat = {0}.", v_hat);

            t_cruising = calculate_cruising_time(veh, v_hat, delta_x_prime); 
            SPDLOG_DEBUG("T cruising = {0}.",t_cruising);

        }
        else {
            v_hat =  calculate_v_hat(veh);
            SPDLOG_DEBUG("V hat = {0}.", v_hat);

            t_cruising = 0.0;
            SPDLOG_DEBUG("T cruising = {0}.",t_cruising);

        }
        
        // calculate planned acceleration time interval
        double t_accel = calculate_acceleration_time(veh, v_hat);
        SPDLOG_DEBUG("T accel = {0}.",t_accel);

        // calculate planned deceleration time interval
        double t_decel = calculate_deceleration_time(veh, v_hat);
        SPDLOG_DEBUG("T decel = {0}.",t_decel);

        // calculate time to stop bar
        double time_to_stop_bar = t_accel + t_cruising + t_decel;
        return static_cast<uint64_t>(ceil(time_to_stop_bar * 1000.0)) + veh._cur_time;
    }

    bool all_stop_vehicle_scheduler::consider_departure_position_permutation( 
                                                                                const std::list<streets_vehicles::vehicle> &rdvs, 
                                                                                intersection_schedule &option,
                                                                                int starting_departure_position ) const {
        for ( const auto &veh : rdvs ) {
            // If scheduling option moves vehicle departure position more than flexibility limit it is not a valid option.
            if ( abs(starting_departure_position - veh._departure_position) > flexibility_limit ) {
                SPDLOG_WARN(
                    "Not considering scheduling option since change in departure position for vehicle {0} exceeds flexibility limit {1}!",
                    veh._id, flexibility_limit);
                // Break out of scheduling estimation loop and do not consider this scheduling option
                return false;
            }
            vehicle_schedule sched;
            // Populate common vehicle schedule information
            sched.v_id = veh._id;
            // RDVs should have already stopped
            sched.st = veh._actual_st;
            sched.est =  veh._actual_st;
            // set dp position for current permutation                      
            sched.dp = starting_departure_position;
            // Set connection link lanelet id
            sched.link_id = veh._link_id;
            // Set entry lanelet id 
            sched.entry_lane = veh._entry_lane_id;
            // Get vehicle lane info
            OpenAPI::OAILanelet_info veh_lane = get_link_lanelet_info(veh);
            std::shared_ptr<vehicle_schedule> latest_conflicting_vehicle = get_latest_conflicting( veh_lane, option.vehicle_schedules);
            // If there is no previously scheduled DV
            if ( latest_conflicting_vehicle == nullptr) {
                // Check for scheduled RDVs
                if ( option.vehicle_schedules.empty() || option.vehicle_schedules.back().access ) {
                    // Give vehicle access since there are no proceeding vehicles
                    sched.access = true;
                    // Set vehicle state. Will not impact clearance time estimation since set on schedule
                    sched.state = streets_vehicles::vehicle_state::DV;
                    // Entering time equals schedule time
                    sched.et = option.timestamp;
                    // Departure time equals entering time + clearance time
                    sched.dt =  sched.et + estimate_clearance_time( veh, veh_lane);
                }
                else {
                    // Can not grant access to vehicle if 
                    sched.access = false;
                    // Set vehicle state. Will not impact clearance time estimation since set on schedule
                    sched.state = streets_vehicles::vehicle_state::RDV;
                    // Entering time equals schedule time
                    sched.et = option.vehicle_schedules.back().et;
                    // Departure time equals entering time + clearance time
                    sched.dt =  sched.et + estimate_clearance_time( veh, veh_lane); 
                }
            }
            else {
                SPDLOG_DEBUG("Latest conflicting vehicle is {0} and next vehicle is {1}", latest_conflicting_vehicle->v_id, veh._id);
                sched.access = false;
                sched.state =  streets_vehicles::vehicle_state::RDV;
                sched.et =  std::max(latest_conflicting_vehicle->dt, option.vehicle_schedules.back().et);
                // Departure time is estimated clearance time for vehicle and link lane plus entering time
                sched.dt = sched.et + estimate_clearance_time( veh, veh_lane );

                
            }
            // Add vehicle schedule to option
            option.vehicle_schedules.push_back(sched);
            // Point to new previously scheduled vehicle
            // Increment departure position
            starting_departure_position++;

            
        }
        SPDLOG_DEBUG("Schedule Option: \n"+option.toCSV());
        SPDLOG_DEBUG("With delay {0}." ,option.get_delay() );
        return true;

            
    }

    uint64_t all_stop_vehicle_scheduler::estimate_clearance_time( const streets_vehicles::vehicle &veh, 
                                                                    const OpenAPI::OAILanelet_info &link_lane_info) const{
        // Clearance time in seconds 
        double clearance_time = 0;
        // If vehicle is Departing Vehicle consider its location in the link lanelet.
        if ( veh._cur_state == streets_vehicles::vehicle_state::DV ) {
            // Distance covered during constant max acceleration to speed limit
            double constant_acceleration_delta_x = (pow(link_lane_info.getSpeedLimit(), 2) - pow( veh._cur_speed, 2))/(2* veh._accel_max);
            // If vehicle accelerates to speed limit with max acceleration is it still in link lanelet
            if ( veh._cur_distance > constant_acceleration_delta_x ) {
                clearance_time = ( 2 * veh._accel_max * veh._cur_distance - link_lane_info.getSpeedLimit()*veh._cur_speed + pow(veh._cur_speed, 2))/
                    (2*veh._accel_max*link_lane_info.getSpeedLimit());
            } else {
                clearance_time = (sqrt(pow(veh._cur_speed, 2)+2*veh._accel_max*veh._cur_distance) -veh._cur_speed)/
                        veh._accel_max;
            }

        }
        // Consider vehicle is stopped at stop bar
        else  {
            // Distance covered during constant max acceleration to speed limit assuming initial 0 speed.
            double constant_acceleration_delta_x = pow(link_lane_info.getSpeedLimit(), 2) / (2 * veh._accel_max);
            // If vehicle accelerates to speed limit with max acceleration is it still in the link lanelet
            if ( constant_acceleration_delta_x < link_lane_info.getLength()){
                // If yes assume vehicle cruises at speed limit for the duration of the lanelet
                clearance_time = link_lane_info.getLength() / link_lane_info.getSpeedLimit() + 
                            link_lane_info.getSpeedLimit() / (2 *veh._accel_max);
            } else{
                // If not assume vehicle trajectory is constant acceleration from initial speed of 0
                clearance_time = sqrt(2 * link_lane_info.getLength() / veh._accel_max) ;
            }
        }
        // Convert time to milliseconds and round up.
        return static_cast<uint64_t>(ceil(1000.0* clearance_time));
    }

    std::shared_ptr<vehicle_schedule> all_stop_vehicle_scheduler::get_latest_conflicting(const OpenAPI::OAILanelet_info &link_lane, 
                                                                                const std::vector<vehicle_schedule> &schedules) const {
        std::shared_ptr<vehicle_schedule> conflict = nullptr; 

        // Loop through all scheduled vehicles and return conflict with largest dt
        for ( const auto &sched : schedules ) {
            // check if that have conflict with link_lane
            if ( link_lane.getConflictLaneletIds().contains(sched.link_id)) {
                // If no conflicting schedule has been set yet set conflict to first conflicting 
                if ( conflict == nullptr ) {
                    conflict = std::make_shared<vehicle_schedule>( sched);
                }
                else if ( conflict->dt < sched.dt ) {
                    conflict = std::make_shared<vehicle_schedule>( sched);
                }
            }
            
        }
        return conflict;                                              
    }
    void all_stop_vehicle_scheduler::remove_rdv_previously_granted_access( const streets_vehicles::vehicle &veh) {
        auto previously_granted_itr = rdvs_previously_granted_access.begin();
        while ( previously_granted_itr != rdvs_previously_granted_access.end() ) {
            if ( previously_granted_itr->_id == veh._id ) {
                SPDLOG_DEBUG("Removing DV {0} to list of RDVs previously granted access.", previously_granted_itr->_id);
                rdvs_previously_granted_access.erase(previously_granted_itr);
                
            }else {
                previously_granted_itr++;
            }
        }
    }

    bool all_stop_vehicle_scheduler::is_rdv_previously_granted_access( const streets_vehicles::vehicle &veh) const {
        if ( rdvs_previously_granted_access.empty() ) {
            return false;
        }
        else {
            for ( const auto &rdv: rdvs_previously_granted_access ) {
                if ( rdv._id == veh._id ) {
                    return true;
                }
            }
            // not in list
            return false;
        }
    }

    streets_vehicles::vehicle all_stop_vehicle_scheduler::get_vehicle_with_id( const std::list<streets_vehicles::vehicle> &veh_list, const std::string &veh_id ) const {
        streets_vehicles::vehicle rtn;
        bool found_veh = false;
        if ( veh_list.empty() ) {
            throw scheduling_exception("Vehicle List is empty!");
            
        }
        else {
            for (const auto &veh : veh_list ) {
                if ( veh._id == veh_id ) {
                    rtn = veh; 
                    found_veh = true;
                }
            }
        }
        if (!found_veh) {
            throw scheduling_exception("Vehicle" +  veh_id + " not found!");
 
        }
        return rtn;
    }
}