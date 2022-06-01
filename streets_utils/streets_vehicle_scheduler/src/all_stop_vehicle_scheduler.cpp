#include "all_stop_vehicle_scheduler.h"

namespace streets_vehicle_scheduler {

   

    void all_stop_vehicle_scheduler::schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                            intersection_schedule &schedule) {
        if ( !vehicles.empty() ) {
            // Estimate Vehicles at common time 
            estimate_vehicles_at_common_time( vehicles, schedule.timestamp);
            // Create vectors of EVs RDVs and DVs
            std::list<streets_vehicles::vehicle> DVs;
            std::list<streets_vehicles::vehicle> RDVs;
            std::list<streets_vehicles::vehicle> EVs;
            for ( auto it = vehicles.begin(); it != vehicles.end(); it ++ ) {
                streets_vehicles::vehicle veh =it->second;
                if ( veh._cur_state == streets_vehicles::vehicle_state::DV ) {
                    DVs.push_back(veh);
                }
                else if ( veh._cur_state == streets_vehicles::vehicle_state::RDV ) {
                    RDVs.push_back(veh);
                }
                else if ( veh._cur_state == streets_vehicles::vehicle_state::EV) {
                    EVs.push_back(veh);
                }
            }
            
            SPDLOG_INFO("Vehicles to schedule are DVs : {0}, RDVs : {1}, EVs {2} ", DVs.size(), RDVs.size(), EVs.size());

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
        else {
            SPDLOG_DEBUG("No vehicles to schedule.");
        }

    }

    double all_stop_vehicle_scheduler::estimate_delta_x_prime(const streets_vehicles::vehicle &veh, 
                                                                const OpenAPI::OAILanelet_info &entry_lane) const{
        if ( veh._cur_state == streets_vehicles::vehicle_state::EV ) {
            // Get Entry Lanelet
            double delta_x_prime = (pow(entry_lane.getSpeedLimit()*MPH_TO_METER_PER_SECOND,2)-pow(veh._cur_speed,2))/(2*veh._accel_max) - 
                pow(entry_lane.getSpeedLimit()*MPH_TO_METER_PER_SECOND, 2)/(2*veh._decel_max); 
            return  delta_x_prime;
        }
        else {
            SPDLOG_WARN("Cannot estimate delta x prime for vehicle {0} because it's state {1} != EV !" );
        }
    }

    double all_stop_vehicle_scheduler::calculate_v_hat(const streets_vehicles::vehicle &veh) const {
        double numerator = veh._decel_max *( 2 * veh._cur_distance*veh._accel_max + pow(veh._cur_speed, 2));
        // TODO: Are both values positive
        double denominator = veh._decel_max-veh._accel_max;
        return sqrt(numerator/denominator);
    }

    double all_stop_vehicle_scheduler::calculate_acceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const {
        return (v_hat-veh._cur_speed)/veh._accel_max;
    }

    double all_stop_vehicle_scheduler::calculate_cruising_time( const streets_vehicles::vehicle &veh, const double v_hat, const double delta_x_prime) const {
        return (veh._cur_distance - delta_x_prime)/v_hat; 
    }

    double all_stop_vehicle_scheduler::calculate_deceleration_time( const streets_vehicles::vehicle &veh, const double v_hat) const {
        return v_hat/veh._decel_max;
    }

    void all_stop_vehicle_scheduler::schedule_dvs( const std::list<streets_vehicles::vehicle> &dvs,
                                                intersection_schedule &schedule) const{
        for ( auto departing_veh : dvs ) {
            // get lane info
            OpenAPI::OAILanelet_info lane_info =  get_link_lanelet_info( departing_veh );
            // Distance covered assuming constant max acceleration to speed limit
            double constant_acceleration_distance = (pow( lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND, 2) - pow( departing_veh._cur_speed, 2))/
                (2*departing_veh._accel_max);
            double dt;
            // If distance covered is larger than lanelet length.
            if ( constant_acceleration_distance > lane_info.getLength() ) {
                // Departure time assuming constant acceleration for entire trajectory
                dt =  ( 2 * departing_veh._accel_max * departing_veh._cur_distance - lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND*departing_veh._cur_speed + pow(departing_veh._cur_speed, 2))/
                    (2*departing_veh._accel_max*lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND);
            }else {
                // Departure time assuming constant acceleration period followed by cruising at lanelet speed limit period
                dt =  (sqrt(pow(departing_veh._cur_speed, 2)+2*departing_veh._accel_max*departing_veh._cur_distance) -departing_veh._cur_speed)/
                        departing_veh._accel_max;
            }
            vehicle_schedule veh_sched;
            // set id
            veh_sched.v_id = departing_veh._id;
            // set est (TODO is it correct to set est to actual st)
            veh_sched.est = departing_veh._actual_st;
            // set st
            veh_sched.st = departing_veh._actual_st;
            // set et
            veh_sched.et =  departing_veh._actual_et;
            // set estimated dt in milliseconds
            veh_sched.dt =  ceil(dt*1000);
            // set departure position
            veh_sched.dp = departing_veh._departure_position;
            // set state
            veh_sched.state = departing_veh._cur_state;
            // set vehicle link id
            veh_sched.link_id =  departing_veh._link_id;
            // set vehicle entry lane id
            veh_sched.entry_lane =  departing_veh._entry_lane_id;
            // add vehicle_schedule
            schedule.vehicle_schedules.push_back(veh_sched);

        }

    }

    void all_stop_vehicle_scheduler::schedule_rdvs( std::list<streets_vehicles::vehicle> &rdvs, intersection_schedule &schedule ) const {
        // Vector of considered intersection_schedule_options
        std::vector<intersection_schedule> schedule_options;
        // Sort rdvs ascending order based on departure position
        rdvs.sort(departure_position_comparator);
        // Earliest possible departure position is equal to first RDVs departure position
        int starting_departure_position = rdvs.begin()->_departure_position;
        SPDLOG_DEBUG("Staring the schedule RDVs from departure index {0}!", starting_departure_position );
        // TODO: for 8 possible approaches this may need to be optimized (greedy search)
        do { 
            // Index to assign RDVs in every possible departure order permutation
            int proposed_departure_position =  starting_departure_position;
            // update schedule option to include already scheduled vehicles
            intersection_schedule option = schedule;
        
            // previous vehicle is equal to last scheduled DV
            std::shared_ptr<vehicle_schedule> previous_vehicle = nullptr;
            if ( !schedule.vehicle_schedules.empty() ) {
                previous_vehicle = std::make_shared<vehicle_schedule> (schedule.vehicle_schedules.back());
            }
            // Pointer to previous scheduled vehicle for departure time  and entering time 
            bool valid_option =  consider_departure_position_permutation(rdvs, option, starting_departure_position, previous_vehicle);
            //fix an RDV based on lower bound delay
            if ( valid_option ) {
                schedule_options.push_back(option);
            }

        }
        // While ( !all RDVS schedule)
        while (std::next_permutation( rdvs.begin(), rdvs.end(), departure_position_comparator));
        // Sort in ascending order based on delay.
        std::sort(schedule_options.begin(), schedule_options.end(), delay_comparator);
        // Add scheduled RDVs starting at last scheduled DV position to schedule
        for (auto sched : schedule_options.front().vehicle_schedules){
            if (sched.dp == starting_departure_position ){
                schedule.vehicle_schedules.push_back( sched );
                starting_departure_position ++;
            }
        }

    }

    void all_stop_vehicle_scheduler::schedule_evs( std::list<streets_vehicles::vehicle> &evs,intersection_schedule &schedule ) const {
        SPDLOG_INFO("Scheduling EVs ");
        // Map of entry lane ids to preceeding already scheduled vehicle
        std::unordered_map<int , vehicle_schedule> preceding_vehicle_entry_lane_map;
        for ( auto entry_lane : intersection_info->getEntryLanelets() ) {
            int lane_id = entry_lane.getId();
            vehicle_schedule preceding_veh;
            // Only one RDV can exist for each approach
            for (auto veh_sched : schedule.vehicle_schedules ) {
                if ( veh_sched.state == streets_vehicles::vehicle_state::RDV) {
                    // All RDVs
                    preceding_veh = veh_sched;
                    preceding_vehicle_entry_lane_map.insert({lane_id, preceding_veh});  

                }
            }
        }
        SPDLOG_INFO("Created map of preceeding vehicles in each lane");
        if ( !preceding_vehicle_entry_lane_map.empty()) {
            for ( auto map_entry : preceding_vehicle_entry_lane_map ) {
                SPDLOG_INFO("Preceding Vehicle {0} in lane {1}", map_entry.second.v_id, map_entry.first);
            }
        }
        // Sort vehicles based on distance
        evs.sort(departure_position_comparator);
        

        std::unordered_map<int, std::list<streets_vehicles::vehicle>> vehicle_to_be_scheduled_next;
        for ( auto entry_lane : intersection_info->getEntryLanelets() ) {
            std::list<streets_vehicles::vehicle> vehicles_in_lane;
            for ( auto ev : evs ) {
                if ( ev._entry_lane_id == static_cast<int>(entry_lane.getId())) {
                    vehicles_in_lane.push_back(ev);
                }
            }
            if ( !vehicles_in_lane.empty())
                vehicle_to_be_scheduled_next.insert({ static_cast<int>(entry_lane.getId()), vehicles_in_lane });
        }
        SPDLOG_INFO("Map of vehicles to be scheduled next is populated");
        if ( !vehicle_to_be_scheduled_next.empty()) {
            for ( auto map_entry : vehicle_to_be_scheduled_next ) {
                SPDLOG_INFO("Next Vehicle {0} to be scheduled in lane {1}", map_entry.second.front()._id, map_entry.first);
            }
        }
        do {
            vehicle_schedule sched;
            // Calculate ST for each next vehicle in each lane and schedule the vehicle with the lowest ST
            uint64_t lowest_st = 0;

            //Populate Map
            // Find next vehicle for each entry lanelet
            for ( auto map_entry : vehicle_to_be_scheduled_next ) {
                // Take first vehicle to be scheduled in lane.
                streets_vehicles::vehicle ev = map_entry.second.front();
                SPDLOG_INFO( "Estimating schedule for {0}.", ev._id);

                // Get link lanelet information for ev
                OpenAPI::OAILanelet_info link_lane = get_link_lanelet_info( ev );
                SPDLOG_INFO( "Link lanelet for {0} is {1}.", ev._id, link_lane.getId());
                // Calculate EST for vehicle
                uint64_t est = estimate_earliest_time_to_stop_bar(ev);
                SPDLOG_INFO( "EST for vehicle {0} is {1}." ,ev._id, est ) ;
                // Store ST value for vehicle
                uint64_t st;


                // If there is a preceeding vehicle scheduled in this lane
                if ( preceding_vehicle_entry_lane_map.find( ev._cur_lane_id )  != preceding_vehicle_entry_lane_map.end() ) {
                    // Get preceeding vehicle in lane
                    vehicle_schedule preceding_veh = preceding_vehicle_entry_lane_map.find( ev._cur_lane_id )->second;
                    SPDLOG_INFO("Preceding vehicle schedule in lane {0} is {1}.", preceding_veh.entry_lane, preceding_veh.v_id);
                    // If there is a preceeding vehicle the st is calculated as max of EST and preceeding vehicle ET plus a time
                    // buffer to account for the time it takes the preceeding vehicle to enter the intersection
                    st =  std::max( sched.est, preceding_veh.et + entering_time_buffer );

                }
                // IF there is no preceeding vehicle ST == EST
                else {
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

                    // Get previously scheduled vehicle regardless of lane
                    if ( !schedule.vehicle_schedules.empty() ) {
                        vehicle_schedule previously_scheduled = schedule.vehicle_schedules.back();
                        SPDLOG_INFO( "Last schedule vehicle is {0} in entry lane {1} using link {2}.");
                        // If there is a previously scheduled vehicle with conflicting direction
                        if ( link_lane.getConflictLaneletIds().contains(static_cast<qint32>(previously_scheduled.link_id))) {
                            // Entering time is the maximum between the conflicting vehicles departure time and the current vehicles stopping time
                            sched.et = std::max(previously_scheduled.dt, sched.st);
                        }else{
                            //TODO: Should this be max of st and previous et or just st?
                            sched.et = std::max( sched.st, previously_scheduled.et);
                        }
                    }
                    else {
                        sched.et = sched.st;
                    }
                    // Departure time is equal to entering time + clearance time
                    sched.dt = sched.et + estimate_clearance_time( ev, link_lane );
            
                }
               
            }
            SPDLOG_INFO( "Found vehicle {0} with lowest stopping time {1} in lane {2}", sched.v_id, sched.st, sched.entry_lane);
            // Add lowest ST to schedule
            schedule.vehicle_schedules.push_back(sched);
            // Replace previous preceeding vehicle for lane with scheduled vehicle
            if ( preceding_vehicle_entry_lane_map.find(sched.entry_lane) != preceding_vehicle_entry_lane_map.end()) {
                preceding_vehicle_entry_lane_map.find(sched.entry_lane)->second =  sched;
            }
            else {
                preceding_vehicle_entry_lane_map.insert({sched.entry_lane, sched});
            }
            // Pop vehicle off of list of vehicles to be scheduled for each lane
            vehicle_to_be_scheduled_next.find(sched.entry_lane)->second.pop_front();
            // If pop makes list empty remove map entry
            if ( vehicle_to_be_scheduled_next.find(sched.entry_lane)->second.empty() ) {
                SPDLOG_INFO("All vehicles in approach lanelet {0} have been scheduled!", sched.entry_lane);
                vehicle_to_be_scheduled_next.erase(sched.entry_lane);
            }
            
        }
        // Schedule EVs in order of est
        while ( !vehicle_to_be_scheduled_next.empty() );
        SPDLOG_INFO("All EV lists in every entry lane have been scheduled!");


    }

    uint64_t all_stop_vehicle_scheduler::estimate_earliest_time_to_stop_bar(const streets_vehicles::vehicle &veh) const{
        // Distance to stop bar TODO:Is this incorrect?
        double delta_x = veh._cur_distance;
        // Get Entry Lane
        OpenAPI::OAILanelet_info lane_info =  get_entry_lanelet_info( veh );
        SPDLOG_INFO("Get entry lane information {0} for vehicle {1}.",  lane_info.getId(), veh._id);
        // Distance necessary to get to max speed and decelerate with decel_max
        double delta_x_prime =  estimate_delta_x_prime( veh, lane_info );
        SPDLOG_INFO("Delta X = {0}.", delta_x_prime);

        // Calculate v_hat and planned cruising time interval
        double v_hat;
        double t_cruising;
        if ( delta_x >= delta_x_prime ) {
            v_hat = lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND;
            SPDLOG_INFO("V hat = {0}.", v_hat);

            t_cruising = calculate_cruising_time(veh, v_hat, delta_x_prime); 
            SPDLOG_INFO("T cruising = {0}.",t_cruising);

        }
        else {
            v_hat =  calculate_v_hat(veh);
            SPDLOG_INFO("V hat = {0}.", v_hat);

            t_cruising = 0.0;
            SPDLOG_INFO("T cruising = {0}.",t_cruising);

        }
        // calculate planned acceleration time interval
        double t_accel = calculate_acceleration_time(veh, v_hat);
        // calculate planned deceleration time interval
        double t_decel = calculate_deceleration_time(veh, v_hat);
        // calculate time to stop bar
        double time_to_stop_bar = t_accel + t_cruising + t_decel;
        
        return ceil(time_to_stop_bar * 1000) + veh._cur_time;
    }

    bool all_stop_vehicle_scheduler::consider_departure_position_permutation( 
                                                                                const std::list<streets_vehicles::vehicle> &rdvs, 
                                                                                intersection_schedule &option,
                                                                                int starting_departure_position,
                                                                                std::shared_ptr<vehicle_schedule> previously_scheduled_vehicle ) const {
        for ( auto veh : rdvs ) {
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
            // If there is no previously scheduled vehicle
            if ( previously_scheduled_vehicle == nullptr) {
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
                // If there is conflicting directions with previous vehicle
                if ( veh_lane.getConflictLaneletIds().contains(static_cast<qint32>(previously_scheduled_vehicle->link_id))) {
                    // For conflict entering time needs to be the previously scheduled vehicles dt
                    sched.et =  previously_scheduled_vehicle->dt;
                    // Vehicle cannot enter since it has conflict with previous vehicle
                    sched.state = streets_vehicles::vehicle_state::RDV;
                    // Departure time is estimated clearance time for vehicle and link lane plus entering time
                    sched.dt = sched.et + estimate_clearance_time( veh, veh_lane );
                    // set access to false
                    sched.access = false;

                } else {
                    // Entering time for RDVs should be max of entering time of previous vehicle and schedule timestamp since they
                    // can only enter the intersection once they have received a schedule allowing them to enter the intersection
                    sched.et =  std::max(previously_scheduled_vehicle->et, option.timestamp);
                    // Set vehicle state. Will not impact clearance time estimation since set on schedule
                    sched.state =  streets_vehicles::vehicle_state::DV;
                    // Departure time is estimated clearance time for vehicle and link lane plus entering time
                    sched.dt = sched.et + estimate_clearance_time( veh, veh_lane );
                    // Set access to true
                    sched.access = true;

                }
            }
            // Add vehicle schedule to option
            option.vehicle_schedules.push_back(sched);
            // Point to new previously scheduled vehicle
            previously_scheduled_vehicle = std::make_shared<vehicle_schedule>(sched);
            // Increment departure position
            starting_departure_position++;
            
            
        }
            
    }

    uint64_t all_stop_vehicle_scheduler::estimate_clearance_time( const streets_vehicles::vehicle &veh, 
                                                                    const OpenAPI::OAILanelet_info &link_lane_info) const{
        // Clearance time in seconds 
        double clearance_time = 0;
        // If vehicle is Departing Vehicle consider its location in the link lanelet.
        if ( veh._cur_state == streets_vehicles::vehicle_state::DV ) {
            // Distance covered during constant max acceleration to speed limit
            double constant_acceleration_delta_x = (pow(link_lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND, 2) - pow( veh._cur_speed, 2))/(2* veh._accel_max);
            // If vehicle accelerates to speed limit with max acceleration is it still in link lanelet
            if ( veh._cur_distance > constant_acceleration_delta_x ) {
                clearance_time = ( 2 * veh._accel_max * veh._cur_distance - link_lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND*veh._cur_speed + pow(veh._cur_speed, 2))/
                    (2*veh._accel_max*link_lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND);
            } else {
                clearance_time = (sqrt(pow(veh._cur_speed, 2)+2*veh._accel_max*veh._cur_distance) -veh._cur_speed)/
                        veh._accel_max;
            }

        }
        // Consider vehicle is stopped at stop bar
        else  {
            // Distance covered during constant max acceleration to speed limit assuming initial 0 speed.
            double constant_acceleration_delta_x = pow(link_lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND, 2) / (2 * veh._accel_max);
            // If vehicle accelerates to speed limit with max acceleration is it still in the link lanelet
            if ( constant_acceleration_delta_x < link_lane_info.getLength()){
                // If yes assume vehicle cruises at speed limit for the duration of the lanelet
                clearance_time = link_lane_info.getLength() / link_lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND + 
                            link_lane_info.getSpeedLimit()*MPH_TO_METER_PER_SECOND / (2 *veh._accel_max);
            } else{
                // If not assume vehicle trajectory is constant acceleration from initial speed of 0
                clearance_time = sqrt(2 * link_lane_info.getLength() / veh._accel_max) ;
            }
        }
        // Convert time to milliseconds and round up.
        return ceil(1000* clearance_time);
    }
}