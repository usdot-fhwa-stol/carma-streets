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

            virtual uint64_t get_timeout() const = 0;

            virtual void set_timeout(uint64_t timeout) = 0;
        
    };
}