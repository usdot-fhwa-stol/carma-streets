#include "vehicle_list.h"

namespace streets_vehicles {


   std::vector<vehicle> vehicle_list::get_vehicles_by_lane( const int lane_id ) {
        std::vector<vehicle> vehicles_in_entry_lane;
        // Read lock
        std::shared_lock  lock(vehicle_list_lock);
        for ( auto it = vehicles.begin(); it != vehicles.end(); it ++ ) {
            vehicle veh =it->second;
            if ( veh._cur_lane_id == lane_id ) {
                vehicles_in_entry_lane.push_back(veh);
            }
        }
        return vehicles_in_entry_lane;
    }

    std::vector<vehicle> vehicle_list::get_vehicles_by_state( const vehicle_state state ) {
        std::vector<vehicle> vehicle_in_state;
        // Read Lock
        std::shared_lock  lock(vehicle_list_lock);
        for ( auto it = vehicles.begin(); it != vehicles.end(); it ++ ) {
            vehicle veh =it->second;
            if ( veh._cur_state == state ) {
                vehicle_in_state.push_back(veh);
            }
        }
        return vehicle_in_state;
    }

    std::unordered_map<std::string,vehicle> vehicle_list::get_vehicles() {
        // Read Lock
        std::shared_lock  lock(vehicle_list_lock);
        return vehicles;
    }

    void vehicle_list::add_vehicle(const vehicle &veh) {
        vehicles.insert(std::pair<std::string, vehicle>({veh._id,veh}));
    }

    void vehicle_list::update_vehicle(const vehicle &vehicle) {
        auto it = vehicles.find(vehicle._id);
        if (it != vehicles.end()) {
            it->second = vehicle;
        }else{
            SPDLOG_WARN("Did not find vehicle {0} to update!", vehicle._id);
        }
    }


    void vehicle_list::purge_old_vehicles( const uint64_t timeout ) {
        uint64_t timeout_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeout;
        for ( auto it = vehicles.begin(); it != vehicles.end();  ) {
            vehicle veh =it->second;
            SPDLOG_DEBUG("Checking Vehicle {0} timestamp {1} < timeout time {2} !", veh._id, veh._cur_time, timeout );
            if ( veh._cur_time < timeout_time  ) {
                SPDLOG_WARN("Vehicle {0} timed out!", veh._id);
                vehicles.erase(it ++);
            }
            else {
                it ++;
            }
        }

    }

    void vehicle_list::process_update( const std::string &update ) {
      
        if ( processor != nullptr ) {
            // Write lock for purge/update/add
            std::unique_lock  lock(vehicle_list_lock);
            purge_old_vehicles( processor->get_timeout());
            try{
                vehicle vehicle;
                rapidjson::Document doc;
                std::string v_id = processor->get_vehicle_id(update, doc);
                if ( vehicles.find(v_id) != vehicles.end() ) {
                    // If vehicle is already in Vehicle List, update vehicle
                    vehicle = vehicles.find(v_id)->second;
                    processor->process_status_intent( doc, vehicle);
                    update_vehicle(vehicle);
                    SPDLOG_DEBUG("Update Vehicle : {0}" , vehicle.get_id());
                }
                else {
                    // If vehicle is not already in Vehicle list, add vehicle
                    processor->process_status_intent( doc, vehicle);
                    add_vehicle(vehicle);
                    SPDLOG_DEBUG("Added Vehicle : {0}" , vehicle.get_id());

                }
            }
            catch( const status_intent_processing_exception &ex) {
                SPDLOG_CRITICAL("Failed to parse status and intent update: {0}", ex.what());
            }
        }
        else {
            SPDLOG_CRITICAL("No status_intent_processor available! Set status_intent_processor for vehicle_list!");
        }
        
    }

    void vehicle_list::set_processor(std::shared_ptr<status_intent_processor> _processor ) {
        // Write Lock
        processor = _processor;
    }

    std::shared_ptr<status_intent_processor> vehicle_list::get_processor() {
        // Read lock
        return processor;
    }

    void vehicle_list::clear() {
        // Write Lock
        std::unique_lock  lock(vehicle_list_lock);
        SPDLOG_WARN("Clearing Vehicle list!");
        vehicles.clear();

    }

    
}