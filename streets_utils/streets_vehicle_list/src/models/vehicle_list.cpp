#include "vehicle_list.h"

namespace streets_vehicles {

    vehicle_list::vehicle_list() {
    }

    const std::vector<vehicle> vehicle_list::get_vehicles_by_lane(int lane_id ) {
        auto &instance = get_singleton();
        std::vector<vehicle> vehicles_in_entry_lane;
        for ( auto it = instance.vehicles.begin(); it != instance.vehicles.end(); it ++ ) {
            vehicle veh =it->second;
            if ( veh.get_cur_lane_id() == lane_id ) {
                vehicles_in_entry_lane.push_back(veh);
            }
        }
        return vehicles_in_entry_lane;
    }

    const std::vector<vehicle> vehicle_list::get_vehicles_by_state( vehicle_state state ) {
        auto &instance = get_singleton();
        std::vector<vehicle> vehicle_in_state;
        for ( auto it = instance.vehicles.begin(); it != instance.vehicles.end(); it ++ ) {
            vehicle veh =it->second;
            if ( veh.get_cur_state() == state ) {
                vehicle_in_state.push_back(veh);
            }
        }
        return vehicle_in_state;
    }

    const std::unordered_map<std::string,vehicle> vehicle_list::get_vehicles() {
        auto &instance = get_singleton();
        return instance.vehicles;
    }

    void vehicle_list::add_vehicle(const vehicle &veh) {
        std::unique_lock<std::mutex>(vehicle_list_lock);
        vehicles.insert(std::pair<std::string, vehicle>({veh.get_id(),veh}));
    }

    void vehicle_list::update_vehicle(const vehicle &vehicle) {
        auto it = vehicles.find(vehicle.get_id());
        if (it != vehicles.end()) {
            std::unique_lock<std::mutex>(vehicle_list_lock);
            it->second = vehicle;
        }else{
            SPDLOG_WARN("Did not find vehicle {0} to update!", vehicle.get_id());
        }
    }

    void vehicle_list::remove_vehicle(const std::string &v_id ){
        std::unique_lock<std::mutex>(vehicle_list_lock);
        vehicles.erase(v_id);
    }

    void vehicle_list::purge_old_vehicles( int timeout ) {
        int timeout_time = std::chrono::system_clock::now().time_since_epoch().count() - timeout;
        for ( auto it = vehicles.begin(); it != vehicles.end(); it ++ ) {
            vehicle veh =it->second;
            if ( veh.get_cur_time() < timeout_time ) {
                remove_vehicle( veh.get_id() );
            }
        }

    }

    void vehicle_list::process_update( const std::string &update ) {
        auto &instance = get_singleton();
        if ( instance.processor != nullptr ) {
            try{
                vehicle vehicle;
                instance.processor->process_status_intent( update, vehicle);
                if ( instance.vehicles.find(vehicle.get_id()) != instance.vehicles.end() ) {
                    instance.update_vehicle(vehicle);
                }
                else {
                    instance.add_vehicle(vehicle);
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

    void vehicle_list::set_processor(std::unique_ptr<status_intent_processor> processor ) {
        auto &instance = get_singleton();
        instance.processor = std::move(processor);
    }






    
}