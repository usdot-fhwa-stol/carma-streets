#pragma once

#include "vehicle.h"
#include "status_intent_processing_exception.h"
#include "streets_singleton.h"
#include "status_intent_processor.h"
#include "status_intent_processing_exception.h"
#include <chrono>  


using namespace streets_vehicles;

namespace streets_vehicles {
    /**
     * @brief Streets Singleton used to store and update vehicle information for all
     * vehicles in an intersection. Consumes status and intent JSON vehicle updates and
     * populates a map of vehicles.
     * 
     */
    class vehicle_list : public streets_service::streets_singleton<vehicle_list>  {
        friend class streets_singleton<vehicle_list>;

        protected:
            std::unordered_map<std::string, vehicle> vehicles;
            std::mutex vehicle_list_lock;

            std::unique_ptr<status_intent_processor> processor;
            /**
             * @brief Adds a vehicle to the vehicle map.
             * 
             * @param vehicle to add.
             */
            void add_vehicle(const vehicle &vehicle);
            /**
             * @brief Removes a vehicle from the vehicle map.
             * 
             * @param v_id to remove 
             */
            void remove_vehicle (const std::string &v_id);
            /**
             * @brief Updates a vehicle in the vehicle map, with new vehicle information.
             * 
             * @param vehicle new vehicle information to update vehicle with.
             */
            void update_vehicle(const vehicle &vehicle);
            /**
             * @brief Removes all vehicles in map that have not been updated in timeout period.
             * 
             * @param timeout time in milliseconds from current time after which vehicles will be removed from the vehicle list.
             */
            void purge_old_vehicles(int timeout);
            
            // Hide get_singleton method. Use static methods instead.
            using streets_singleton::get_singleton;
            

        public:
            /**
             * @brief Construct a new vehicle list object
             * 
             */
            vehicle_list();
            /**
             * @brief Get the vehicles map.
             * 
             * @return const std::unordered_map<std::string, vehicle> .
             */
            static const std::unordered_map<std::string, vehicle> get_vehicles();
            /**
             * @brief Get the vehicles by lane id.
             * 
             * @param lane_id lanelet2 map lane id.
             * @return const std::vector<vehicle> 
             */
            static const std::vector<vehicle> get_vehicles_by_lane(int lane_id);
            /**
             * @brief Get the vehicles by state. 
             * 
             * @param state 
             * @return const std::vector<vehicle> 
             */
            static const std::vector<vehicle> get_vehicles_by_state(vehicle_state state);
            /**
             * @brief Process JSON status and intent update into vehicle update and modifies 
             * vehicle map with update
             * 
             * @param update std::string status and intent JSON vehicle update 
             */
            static void process_update(const std::string &update);
            /**
             * @brief Set the status_intent_processor to allow for customizable update processing.
             * 
             * @param processor status_intent_processor
             */
            static void set_processor(std::unique_ptr<status_intent_processor> processor);
             

    };
}