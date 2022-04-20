#pragma once

#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

#include "streets_vehicle.h"
#include "status_intent_processor.h"

using namespace street_vehicles;
namespace streets_vehicles {

    class all_stop_status_intent_processor : public status_intent_processor{
        public:
            /**
             * @brief Process JSON status and intent update and convert it to
             * an streets_vehicle_update.
             * 
             * @param status_intent_msg rapidjson::Value JSON status and intent update.
             * @return streets_vehicle_update. 
             */ 
            const void from_json( const rapidjson::GenericObject<true, rapidjson::Value> &json, streets_vehicle &vehicle);
            /**
             * @brief Set stopping_distance stopping condition.
             * @param stopping_distance max distance from stop line at which vehicle can be considered stopped in meters.
             */ 
            void set_stopping_distance(double stopping_distance);
            /**
             * @brief Set stopping_speed stopping condition.
             * @param stopping_speed max speed at which vehicle can be considered stopped in m/s.
             */
            void set_stopping_speed(double stopping_speed); 

            void from_json(const rapidjson::GenericObject<true,rapidjson::Value> &json, streets_vehicle &vehicle) const override;

        protected:

            void read_metadata(const rapidjson::GenericObject<true,rapidjson::Value> &json, streets_vehicle &vehicle) const;

            void read_payload(const rapidjson::GenericObject<true,rapidjson::Value> &json, streets_vehicle &vehicle) const;

            void read_est_path(const rapidjson::GenericArray<true,rapidjson::Value> &json, streets_vehicle &vehicle) const;

            void update_vehicle_state(streets_vehicle &vehicle) const;

        private:
            /* max distance from stop line at which a vehicle can be considered stopped */
            double stopping_distance;
            /* max speed, in m/s, at which a vehicle can be considered stopped */
            double stopping_speed;

            bool is_vehicle_stopped(const streets_vehicle &vehicle) const;



    };
}