#pragma once

#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

#include "vehicle.h"
#include "status_intent_processing_exception.h"

using namespace streets_vehicles;


namespace streets_vehicles {
    class status_intent_processor{
        public:
            /**
             * @brief Process JSON status and intent update and convert it to
             * an streets_vehicle_update.
             * 
             * @param status_intent_msg rapidjson::Value JSON status and intent update.
             * @param vehicle vehicle to update.
             * @return streets_vehicle_update. 
             */ 
            void process_status_intent(const rapidjson::Value &status_intent_msg, vehicle &vehicle) const;
            /**
             * @brief Process JSON status and intent update and convert it to
             * an streets_vehicle_update.
             * 
             * @param status_intent_msg string JSON status and intent update.
             * @param vehicle vehicle to update.
             * @return streets_vehicle_update. 
             */ 
            void process_status_intent(const std::string &status_intent_msg, vehicle &vehicle) const;
            /**
             * @brief Process JSON status and intent Object and update vehicle.
             *
             * @param json rapidjson::GenericObject status and intent update.
             * @param vehicle vehicle to update.
             */
            virtual void from_json( const rapidjson::GenericObject<true, rapidjson::Value> &json, vehicle &vehicle) const = 0 ;

    };
}