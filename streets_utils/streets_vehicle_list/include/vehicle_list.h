#pragma once

#include "vehicle.h"
#include "status_intent_processing_exception.h"
#include "status_intent_processor.h"
#include "status_intent_processing_exception.h"
#include <chrono>  
#include <shared_mutex>
#include <mutex>



namespace streets_vehicles {
    /**
     * @brief Class to store vehicle information for all vehicles in an intersection. Contains pointer
     * to a status_intent_processor which holds business logic to process status and intent vehicle 
     * updates. To implement custom message processing simply extend status_intent_processor and set 
     * this object to be the message processor for you vehicle list.
     * 
     * @author Paul Bourelly
     */
    class vehicle_list   {

        private:
            // Map to store vehicles with vehicle id string as keys
            std::unordered_map<std::string, vehicle> vehicles;
            // shared mutex to enable read/write locking (requires C++ > 17)
            std::shared_mutex vehicle_list_lock;
            std::shared_ptr<status_intent_processor> processor;
            /**
             * @brief Adds a vehicle to the vehicle map.
             * 
             * @param vehicle to add.
             */
            void add_vehicle(const vehicle &vehicle);
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
            void purge_old_vehicles(const uint64_t timeout);
            
            

        public:
            /**
             * @brief Construct a new vehicle list object
             * 
             */
            vehicle_list() = default;
            /**
             * @brief Get the vehicles map.
             * 
             * @return std::unordered_map<std::string, vehicle> .
             */
            std::unordered_map<std::string, vehicle> get_vehicles();
            /**
             * @brief Get the vehicles by lane id.
             * 
             * @param lane_id lanelet2 map lane id.
             * @return std::vector<vehicle> 
             */
            std::vector<vehicle> get_vehicles_by_lane(const int lane_id);
            /**
             * @brief Get the vehicles by state. 
             * 
             * @param state 
             * @return std::vector<vehicle> 
             */
            std::vector<vehicle> get_vehicles_by_state(const vehicle_state state);
            /**
             * @brief Process JSON status and intent update into vehicle update and modifies 
             * vehicle map with update
             * 
             * @param update std::string status and intent JSON vehicle update 
             */
            void process_update(const std::string &update);
            /**
             * @brief Set the status_intent_processor to allow for customizable update processing.
             * 
             * @param processor status_intent_processor
             */
            void set_processor(std::shared_ptr<status_intent_processor> _processor);

            /**
             * @brief Get the processor object
             * 
             * @return std::unique_ptr<status_intent_processor> 
             */
            std::shared_ptr<status_intent_processor> get_processor();
            
            /**
             * @brief Clear vehicle list.
             * 
             */
            void clear();
        
             

    };
}