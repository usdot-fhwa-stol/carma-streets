#include "vehicle_scheduler.h"

namespace streets_vehicle_scheduler {
    std::shared_ptr<OpenAPI::OAIIntersection_info> vehicle_scheduler::get_intersection_info() const{
        return intersection_info;
    }

    void vehicle_scheduler::set_intersection_info( std::shared_ptr<OpenAPI::OAIIntersection_info> _intersection_info) {
        intersection_info = _intersection_info;
    }


    OpenAPI::OAILanelet_info vehicle_scheduler::get_entry_lanelet_info(const streets_vehicles::vehicle &veh) const{
        OpenAPI::OAILanelet_info entry_lane;
        bool is_found = false;
        for (const auto &lanelet : intersection_info->getEntryLanelets() ) {
            int lane_id =lanelet.getId();
            if ( lane_id == veh._entry_lane_id ) {
                entry_lane =  lanelet;
                is_found = true;
            }
            
        }
        if (!is_found) {
            throw scheduling_exception("No entry lane " + std::to_string(veh._cur_lane_id) + " found in intersection info!");
        }
        return entry_lane;

    }

     OpenAPI::OAILanelet_info vehicle_scheduler::get_link_lanelet_info(const streets_vehicles::vehicle &veh) const{
        OpenAPI::OAILanelet_info link_lane;
        bool is_found = false;
        for ( const auto &lanelet : intersection_info->getLinkLanelets() ) {
            int lane_id = lanelet.getId();
            if ( lane_id == veh._link_id ) {
                link_lane =  lanelet;
                is_found = true;
            }
            
        }
        if (!is_found) {
            throw scheduling_exception("No link lane " + std::to_string(veh._cur_lane_id) + " found in intersection info!");
        }
        return link_lane;

    }

    void vehicle_scheduler::estimate_vehicles_at_common_time( std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles, 
                                                                const u_int64_t timestamp) const {
        std::vector<std::string> vehicles_to_remove;
        for ( auto &[v_id, veh]: vehicles) {
            // Time difference in seconds
            double delta_t = (((double)timestamp) - ((double)veh._cur_time))/1000.0;
            SPDLOG_TRACE("Schedule timestamp {0} vs vehicle timestamp {1}.", timestamp, veh._cur_time);
            if ( delta_t > 5 ) {
                SPDLOG_WARN("Vehicle update {0} is older than 5 s and no longer considered for scheduling!", veh._id);
                vehicles_to_remove.push_back(veh._id);
                continue;
            }
            else if ( delta_t > 0.2) {
                SPDLOG_WARN("Vehicle update {0} is older than 200 ms!", veh._id);
            }
            else if ( delta_t < 0) {
                SPDLOG_WARN("Timestamp {0} is earlier that latest vehicle {1} update timestamp {2}. Vehicles can only be scheduled for current or future time."
                            , timestamp, veh._id, veh._cur_time );
                vehicles_to_remove.push_back( veh._id);
                continue;     
            }
            // estimate future speed.
            double v_final = veh._cur_speed + veh._cur_accel*delta_t;
            // estimate change in distance
            double delta_x = ((v_final + veh._cur_speed)/2.0)*delta_t;
            SPDLOG_TRACE("Setting speed for vehicle {0} from {1}m/s to {2}m/s.", 
                            veh._id, 
                            veh._cur_speed, 
                            v_final );
            veh._cur_speed = v_final;
            veh._cur_time =  timestamp;
            // Estimate distance to end of lanelet
            if ( veh._cur_distance - delta_x >= 0.0) {
                SPDLOG_TRACE("Setting distance for vehicle {0} from {1}m to {2}m.", 
                            veh._id, 
                            veh._cur_distance, 
                            veh._cur_distance-delta_x );

                veh._cur_distance -= delta_x;
            }
            else {
                // TODO: Could add lanelet transition estimation with intersection model information.
                veh._cur_distance =  0.0;
            }
            // Remove all future points not older than timestamp for estimation.
            for (auto it = veh._future_info.begin() ; it != veh._future_info.end(); it++ ) {
                if ( timestamp > it->timestamp ) {
                    veh._future_info.erase( it--);
                }
            }

        }
        // Remove Old vehicle from consideration
        for (const auto &vehicle : vehicles_to_remove) {
            vehicles.erase(vehicle);
        }
    }
}