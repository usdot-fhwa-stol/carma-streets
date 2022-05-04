#include "vehicle_list.h"

namespace streets_vehicles {


    const std::vector<vehicle> vehicle_list::get_vehicles_by_lane( const int lane_id ) {
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

    const std::vector<vehicle> vehicle_list::get_vehicles_by_state( const vehicle_state state ) {
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
        uint64_t timeout_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - timeout;
        for ( auto it = vehicles.begin(); it != vehicles.end(); it ++ ) {
            vehicle veh =it->second;
            if ( veh.get_cur_time()*1000 < timeout_time ) {
                SPDLOG_WARN("Vehicle {0} timed out!", veh.get_id());
                remove_vehicle( veh.get_id() );
            }
        }

    }

    void vehicle_list::process_update( const std::string &update ) {
        auto &instance = get_singleton();
        instance.purge_old_vehicles( instance.processor->get_timeout());
        if ( instance.processor != nullptr ) {
            try{
                vehicle vehicle;
                rapidjson::Document doc;
                std::string v_id = instance.processor->get_vehicle_id(update, doc);
                if ( instance.vehicles.find(v_id) != instance.vehicles.end() ) {
                    vehicle = instance.vehicles.find(v_id)->second;
                    instance.processor->process_status_intent( doc, vehicle);
                    instance.update_vehicle(vehicle);
                    SPDLOG_DEBUG("Update Vehicle : {0}" , vehicle.get_id());
                }
                else {
                    instance.processor->process_status_intent( doc, vehicle);
                    instance.add_vehicle(vehicle);
                    SPDLOG_DEBUG("Added Vehicle : {0}" , vehicle.get_id());

                }
            }
            catch( const status_intent_processing_exception &ex) {
                SPDLOG_CRITICAL("Failed to parse status and intent update: {0}", ex.what());
            }
            catch ( const std::exception &ex ) {
                SPDLOG_CRITICAL("Failed to parse status and intent update: {0}", ex.what());
            }

        }
        else {
            SPDLOG_CRITICAL("No status_intent_processor available! Set status_intent_processor for vehicle_list!");
        }
        
    }

    void vehicle_list::set_processor(std::unique_ptr<status_intent_processor> processor ) {
        auto &instance = get_singleton();
        std::unique_lock<std::mutex>(instance.vehicle_list_lock);
        instance.processor = std::move(processor);
    }

    std::unique_ptr<status_intent_processor>& vehicle_list::get_processor() {
        return get_singleton().processor;
    }

    void vehicle_list::clear() {
        auto &instance = get_singleton();
        std::unique_lock<std::mutex>(instance.vehicle_list_lock);
        SPDLOG_WARN("Clearing Vehicle list!");
        instance.vehicles.clear();

    }

    
}