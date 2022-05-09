#pragma once

#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

#include "vehicle.h"
#include "status_intent_processing_exception.h"


namespace streets_vehicles {
    /**
     * @brief Abstract Class to allow for customizable processing of status and intent
     * vehicle updates. Simply inherit status_intent_processor and implement custom
     * from_json logic to override processing logic.
     * 
     * @author Paul Bourelly
     */
    class status_intent_processor{
        public:
            /**
             * @brief Process JSON status and intent update and convert it to
             * an streets_vehicle_update.
             * 
             * @param status_intent_msg string JSON status and intent update.
             * @param vehicle vehicle to update.
             * @return streets_vehicle_update. 
             */ 
            void process_status_intent(const rapidjson::Document &doc, vehicle &vehicle ) const;
            /**
             * @brief Process JSON status and intent Object and update vehicle.
             *
             * @param json rapidjson::GenericObject status and intent update.
             * @param vehicle vehicle to update.
             */
            virtual void from_json( const rapidjson::GenericObject<true, rapidjson::Value> &json, vehicle &vehicle) const = 0 ;
            
            /**
             * @brief Get the vehicle id string from status and intent message.
             * 
             * @param status_intent_msg string status and intent message.
             * @return std::string vehicle id.
             */
            std::string get_vehicle_id(const std::string &status_intent_msg, rapidjson::Document &doc) const;

            /**
             * @brief Get the timeout Any vehicle status and intent
             * updates after this will be considered timed out and any vehicles with timed out updates as their most
             * recent update will be purged from the vehicle list.
             * 
             * @return uint64_t 
             */
            virtual uint64_t get_timeout() const = 0;
            /**
             * @brief Set the timeout in milliseconds for the status_intent_processor. Any vehicle status and intent
             * updates after this will be considered timed out and any vehicles with timed out updates as their most
             * recent update will be purged from the vehicle list.
             * 
             * @param timeout in milliseconds
             */
            virtual void set_timeout(uint64_t timeout) = 0;
            /**
             * @brief Destroy the status intent processor object
             * 
             */
            virtual ~status_intent_processor() = default;
        
    };
}