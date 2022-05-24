#include "all_stop_vehicle_scheduler.h"

namespace streets_vehicle_scheduler {

   

    void all_stop_vehicle_scheduler::schedule_vehicles( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                            intersection_schedule &schedule) {
        std::vector<streets_vehicles::vehicle> DVs;
        std::vector<streets_vehicles::vehicle> RDVs;
        std::vector<streets_vehicles::vehicle> EVs;
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

       
        // std::sort(EVs.begin(), EVs.end(), departure_position_comparator);
        // int departure_index =  1; // starting index
        // for (auto veh : EVs) {
        //     if ( veh._departure_position != -1 ) {
                
        //     }
        // }
        estimate_vehicles_at_common_time( vehicles, schedule.timestamp);
        
        // Schedule DVs
        schedule_dvs( DVs, schedule);
        // Select Departure Order for RDV

        // sort schedule based on est
        // std::sort( schedule.vehicle_schedules.begin(), schedule.vehicle_schedules.end(), compare_est_schedule );
//        estimate_st();

    }

    double all_stop_vehicle_scheduler::estimate_delta_x_prime(const streets_vehicles::vehicle &veh, 
                                                                const OpenAPI::OAILanelet_info &entry_lane) const{
        if ( veh._cur_state == streets_vehicles::vehicle_state::EV ) {
            // Get Entry Lanelet
            OpenAPI::OAILanelet_info entry_lane;
            for ( auto lanelet : intersection_info->getEntryLanelets() ) {
                int lane_id = static_cast<int>(lanelet.getId());
                if ( lane_id == veh._cur_lane_id ) {
                    entry_lane =  lanelet;
                }
            }
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

    

    void all_stop_vehicle_scheduler::estimate_est(const std::vector<streets_vehicles::vehicle> &evs, intersection_schedule &schedule ) const{
        
        for ( auto entering_veh: evs ) {
            // Distance to stop bar TODO:Is this incorrect?
            double delta_x = entering_veh._cur_distance;
            // Get Entry Lane
            OpenAPI::OAILanelet_info lane_info =  get_entry_lanelet_info( entering_veh );
            // Distance necessary to get to max speed and decelerate with decel_max
            double delta_x_prime =  estimate_delta_x_prime( entering_veh, lane_info );
            // Calculate v_hat and planned cruising time interval
            double v_hat;
            double t_cruising;
            if ( delta_x >= delta_x_prime ) {
                v_hat = lane_info.getSpeedLimit();
                t_cruising = calculate_cruising_time(entering_veh, v_hat, delta_x_prime); 

            }
            else {
                v_hat =  calculate_v_hat(entering_veh);
                t_cruising = 0.0;
            }
            // calculate planned acceleration time interval
            double t_accel = calculate_acceleration_time(entering_veh, v_hat);
            // calculate planned deceleration time interval
            double t_decel = calculate_deceleration_time(entering_veh, v_hat);
            // calculate time to stop bar
            double time_to_stop_bar = t_accel + t_cruising + t_decel;
            // create vehicle_sched obj
            vehicle_schedule veh_sched;
            // set id
            veh_sched.v_id = entering_veh._id;
            // set est
            // convert from seconds double to milliseconds u_int64t
            veh_sched.est = schedule.timestamp + std::round(time_to_stop_bar * 1000);
            // Add new vehicle schedule to intersection schedule;
            schedule.vehicle_schedules.push_back(veh_sched);
        }

    }



    void all_stop_vehicle_scheduler::schedule_dvs( const std::vector<streets_vehicles::vehicle> &dvs,
                                                intersection_schedule &schedule) const{
        for ( auto departing_veh : dvs ) {
            // get lane info
            OpenAPI::OAILanelet_info lane_info =  get_link_lanelet_info( departing_veh );
            // Distance covered assuming constant max acceleration to speed limit
            double constant_acceleration_distance = (pow( lane_info.getSpeedLimit(), 2) - pow( departing_veh._cur_speed, 2))/
                (2*departing_veh._accel_max);
            double dt;
            // If distance covered is larger than lanelet length.
            if ( constant_acceleration_distance > lane_info.getLength() ) {
                // Departure time assuming constant acceleration for entire trajectory
                dt =  ( 2 * departing_veh._accel_max * departing_veh._cur_distance - lane_info.getSpeedLimit()*departing_veh._cur_speed + pow(departing_veh._cur_speed, 2))/
                    (2*departing_veh._accel_max*lane_info.getSpeedLimit());
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
            // add vehicle_schedule
            schedule.vehicle_schedules.push_back(veh_sched);

        }

    }

    void all_stop_vehicle_scheduler::schedule_rdvs( std::vector<streets_vehicles::vehicle> &rdvs, intersection_schedule &schedule ) const {
        // Vector of considered intersection_schedule_options
        std::vector<intersection_schedule> schedule_options;
        // Sort rdvs ascending order based on departure position
        std::sort(rdvs.begin(), rdvs.end(), departure_position_comparator);
        // Earliest possible departure position is equal to first RDVs departure position
        int starting_departure_position = rdvs.begin()->_departure_position;
        SPDLOG_DEBUG("Staring the schedule RDVs from departure index {0}!", starting_departure_position );
        // TODO: for 8 possible approaches this may need to be optimized (greedy search)
        do { 
            // Index to assign RDVs in every possible departure order permutation
            int proposed_departure_position =  starting_departure_position;
            // update schedule option to include already scheduled vehicles
            intersection_schedule option = schedule;
            // timestamp for option equals schedule timestamp
            option.timestamp =  schedule.timestamp;

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
        
        std::sort(schedule_options.begin(), schedule_options.end(), delay_comparator);


    }

    bool all_stop_vehicle_scheduler::consider_departure_position_permutation( 
                                                                                const std::vector<streets_vehicles::vehicle> &rdvs, 
                                                                                intersection_schedule &option,
                                                                                int starting_departure_position,
                                                                                std::shared_ptr<vehicle_schedule> previously_scheduled_vehicle ) const {
        for ( auto veh : rdvs ) {
                // If scheduling option moves vehicle departure position more than flexibility limit it is not a valid option.
                if ( abs(starting_departure_position - veh._departure_position) > flexibility_rating ) {
                    SPDLOG_WARN(
                        "Not considering scheduling option since change in departure position for vehicle {0} exceeds flexibility limit {1}!",
                        veh._id, flexibility_rating);
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
        if ( veh._cur_state == streets_vehicles::vehicle_state::RDV) {
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
        else if ( veh._cur_state == streets_vehicles::vehicle_state::DV ) {
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
        // Convert time to milliseconds
        return ceil(1000* clearance_time);
    }
}