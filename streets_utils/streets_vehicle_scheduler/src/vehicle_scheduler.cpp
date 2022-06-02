#include "vehicle_scheduler.h"

namespace streets_vehicle_scheduler {
    std::shared_ptr<OpenAPI::OAIIntersection_info> vehicle_scheduler::get_intersection_info() {
        return intersection_info;
    }

    void vehicle_scheduler::set_intersection_info( std::shared_ptr<OpenAPI::OAIIntersection_info> &_intersection_info) {
        intersection_info = _intersection_info;
    }

    void vehicle_scheduler::set_flexibility_limit( const int limit ) {
        flexibility_limit = limit;
    }

    OpenAPI::OAILanelet_info vehicle_scheduler::get_entry_lanelet_info(const streets_vehicles::vehicle &veh) const{
        OpenAPI::OAILanelet_info entry_lane;
        bool is_found = false;
        for ( auto lanelet : intersection_info->getEntryLanelets() ) {
            int lane_id = static_cast<int>(lanelet.getId());
            if ( lane_id == veh._entry_lane_id ) {
                entry_lane =  lanelet;
                is_found = true;
            }
            
        }
        if (!is_found) {
            throw scheduling_exception("No lane " + std::to_string(veh._cur_lane_id) + " found in intersection info!");
        }
        return entry_lane;

    }

     OpenAPI::OAILanelet_info vehicle_scheduler::get_link_lanelet_info(const streets_vehicles::vehicle &veh) const{
        OpenAPI::OAILanelet_info link_lane;
        bool is_found = false;
        for ( auto lanelet : intersection_info->getLinkLanelets() ) {
            int lane_id = static_cast<int>(lanelet.getId());
            if ( lane_id == veh._link_id ) {
                link_lane =  lanelet;
                is_found = true;
            }
            
        }
        if (!is_found) {
            throw scheduling_exception("No lane " + std::to_string(veh._cur_lane_id) + " found in intersection info!");
        }
        return link_lane;

    }

    void vehicle_scheduler::estimate_vehicles_at_common_time( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                                const u_int64_t timestamp) {
        for ( auto &map_entry: vehicles) {
            // Time difference in seconds
            double delta_t = (double)(timestamp - map_entry.second._cur_time)/1000.0;
            if ( delta_t > 500.0 ) {
                SPDLOG_INFO("Vehicle update {0} is older than 5 times the vehicle update interval (500 ms)!", map_entry.second._id);
            }
            else if ( delta_t > 200.0) {
                SPDLOG_WARN("Vehicle update {0} is older than double the vehicle update interval (200 ms)!", map_entry.second._id);
            }
            // estimate future speed.
            double v_final = map_entry.second._cur_speed + map_entry.second._cur_accel*delta_t;
            // estimate change in distance
            double delta_x = ((v_final + map_entry.second._cur_speed)/2.0)*delta_t;
            SPDLOG_INFO("Setting speed for vehicle {0} from {1}m/s to {2}m/s.", 
                            map_entry.second._id, 
                            map_entry.second._cur_speed, 
                            v_final );
            map_entry.second._cur_speed = v_final;
            SPDLOG_INFO("Speed for vehicle {0} is {1}m/s", 
                            map_entry.second._id, 
                            map_entry.second._cur_speed);
            map_entry.second._cur_time =  timestamp;
            // Estimate distance to end of lanelet
            if ( map_entry.second._cur_distance - delta_x >= 0.0) {
                SPDLOG_INFO("Setting distance for vehicle {0} from {1}m to {2}m.", 
                            map_entry.second._id, 
                            map_entry.second._cur_distance, 
                            map_entry.second._cur_distance-delta_x );

                map_entry.second._cur_distance -= delta_x;
                SPDLOG_INFO("Distance for vehicle {0} is {1}m", 
                            map_entry.second._id, 
                            map_entry.second._cur_distance);
            }
            else {
                // TODO: Could add lanelet transition estimation with intersection model information.
                map_entry.second._cur_distance =  0.0;
            }
            // Remove all future points not older than timestamp for estimation.
            for (auto it = map_entry.second._future_info.begin() ; it != map_entry.second._future_info.end(); it++ ) {
                if ( timestamp > it->timestamp ) {
                    map_entry.second._future_info.erase( it--);
                }
            }

        }
    }
}