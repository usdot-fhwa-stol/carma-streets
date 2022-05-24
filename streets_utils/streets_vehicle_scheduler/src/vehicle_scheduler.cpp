#include "vehicle_scheduler.h"

namespace streets_vehicle_scheduler {
    std::shared_ptr<OpenAPI::OAIIntersection_info> vehicle_scheduler::get_intersection_info() {
        std::shared_lock lock(scheduler_lock);
        return intersection_info;
    }

    void vehicle_scheduler::set_intersection_info( std::shared_ptr<OpenAPI::OAIIntersection_info> &_intersection_info) {
        std::unique_lock lock(scheduler_lock);
        intersection_info = _intersection_info;
    }

    void vehicle_scheduler::estimate_vehicles_at_common_time( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                                const u_int64_t timestamp) {
        for ( auto map_entry: vehicles) {
            // Time difference in seconds
            double delta_t = (double)(timestamp - map_entry.second._cur_time)/1000.0;
            // estimate future speed.
            double v_final = map_entry.second._cur_speed + map_entry.second._cur_accel*delta_t;
            // estimate chang in distance
            double delta_x = ((v_final + map_entry.second._cur_speed)/2.0)*delta_t;

            map_entry.second._cur_speed = v_final;
            map_entry.second._cur_time =  timestamp;
            // Estimate distance to end of lanelet
            if ( map_entry.second._cur_distance - delta_x >= 0.0) {
                map_entry.second._cur_distance =  map_entry.second._cur_distance - delta_x;
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