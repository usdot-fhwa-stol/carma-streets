#pragma once

#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

#include "vehicle.h"
#include "status_intent_processor.h"

namespace streets_vehicles {

    /**
     * @brief Status Intent processor implementation for all stop use case. Updates
     * vehicle states based on the condition that all vehicles must stop before entering
     * a stop sign controlled intersection.
     * 
     * @author Paul Bourelly
     */
    class all_stop_status_intent_processor : public status_intent_processor{
        public:
            /**
             * @brief Construct a new all stop status intent processor object
             * 
             * @param stopping_distance min distance from stop line in meters to be considered stopped(default = 5 m).
             * @param stopping_speed min speed in m/s to be considered stopped(default = 0.1 m/s).
             * @param timeout time interval after which vehicle updates will no longer be considered valid in milliseconds(default = 30s).
             */
            all_stop_status_intent_processor(const double stopping_distance=1.0, const double stopping_speed=0.1, const uint64_t timeout = 30000 ): 
                _stopping_distance(stopping_distance), _stopping_speed(stopping_speed), _timeout(timeout) {
                    SPDLOG_INFO("All stop status and intent processor initialization.");
                };
            /**
             * @brief Set stopping_distance stopping condition.
             * @param stopping_distance max distance from stop line at which vehicle can be considered stopped in meters.
             */ 
            void set_stopping_distance(const double stopping_distance);
            /**
             * @brief Set stopping_speed stopping condition.
             * @param stopping_speed max speed at which vehicle can be considered stopped in m/s.
             */
            void set_stopping_speed(const double stopping_speed); 
            /**
             * @brief Process JSON status and intent update and convert it to
             * an streets_vehicle_update.
             * 
             * @param status_intent_msg rapidjson::Value JSON status and intent update.
             * @return streets_vehicle_update. 
             */ 
            void from_json(const rapidjson::GenericObject<true,rapidjson::Value> &json, vehicle &vehicle) const override;
            /**
             * @brief Get the timeout in milliseconds. Timeout value controls when updates are no longer considered valid.
             * 
             * @return uint64_t timeout value in milliseconds.
             */
            uint64_t get_timeout() const override;
            /**
             * @brief Set the timeout in milliseconds Timeout value controls when updates are no longer considered valid.
             * 
             * @param timeout 
             */
            void set_timeout(const uint64_t timeout) override;

            /**
             * @brief Destroy the all stop status intent processor object
             * 
             */
            ~all_stop_status_intent_processor() override = default;
        protected:
            /**
             * @brief Reads metadata element in status and intent vehicle update.
             * 
             * @param json metadata json object.
             * @param vehicle reference to update.
             */
            void read_metadata(const rapidjson::GenericObject<true,rapidjson::Value> &json, vehicle &vehicle) const;
            /**
             * @brief Reads payload element in status and intent vehicle update.
             * 
             * @param json payload json object.
             * @param vehicle reference to update.
             */
            void read_payload(const rapidjson::GenericObject<true,rapidjson::Value> &json, vehicle &vehicle) const;
            /**
             * @brief Reads estimated path element in status and intent vehicle update.
             * 
             * @param json est_path json object
             * @param vehicle reference to update
             */
            void read_est_path(const rapidjson::GenericArray<true,rapidjson::Value> &json, vehicle &vehicle) const;
            /**
             * @brief Updates vehicle state based on previous state and stopping condition.
             * 
             * @param vehicle reference to update
             */
            virtual void update_vehicle_state(vehicle &vehicle) const;

        private:
            /* max distance from stop line at which a vehicle can be considered stopped */
            double _stopping_distance;
            /* max speed, in m/s, at which a vehicle can be considered stopped */
            double _stopping_speed;
            // Timeout value in milliseconds. Defaults to 30s
            uint64_t _timeout;
            /**
             * @brief Is vehicle considered stopped at stopping line according to stopping conditions.
             * 
             * @param vehicle 
             * @return true if considered stopped .
             * @return false if not considered stopped.
             */
            bool is_vehicle_stopped(const vehicle &vehicle) const;
            



    };
}