#pragma once
#define RAPIDJSON_HAS_STDSTRING 1


#include "signal_phase_and_timing_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>



namespace signal_phase_and_timing {
    struct connection_maneuver_assist{
        /**
         * @brief The common connectionId used by all lanes which this data applies to ( this value 
         * traces to ConnectsTO entries in lanes)
         */
        int connection_id = 0;

        // Expected Clearance Information

        /**
         * @brief The distance from the stop line to the back edge of the last vehicle in queue,
         * as measured along the center line. (Units = meters)
         */
        uint16_t queue_length = 0;

        /**
         * @brief Distance (e.g. beginning from the downstream stop-line up to a given distance) with a high
         * probability for successfully executing the connecting maneuver between two lane during current cycle.
         * Used for enhancing the awareness of vehicles to anticipate if they can pass the stop line of the lane.
         * Used for optimizing the green wave, due to knowledge of vehicles waiting in front of a red light (downstream).
         * The element nextTime in TimeChangeDetails in the containing data frame contains the next timemark at 
         * which an active phase is expected, to form a storage flush interval.
         */
        uint16_t available_storage_length = 0;

        /**
         * @brief If "true", the vehicles on this specific connecting maneuver have to stop on the stop-line and not
         * to enter the collision area.
         */
        bool wait_on_stop;

        /**
         * @brief true if ANY ped or bicycles are detecting crossing the above lanes. Set to false, ONLY if there is a
         * high certainty that there are none present, otherwise element is not sent.
         */
        bool ped_bicycle_detect;

        /**
         * @brief Serialize Connection Maneuver Assist object to rapidjson::Value for writing as JSON string
         * 
         * @param allocator  for writing JSON object.
         * @return rapidjson::Value serialize Connection Maneuver Assist object.
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;
        /**
         * @brief Deserialize Connection Maneuver Assist JSON into Connection Maneuver Assist object.
         * 
         * @param val Connection Maneuver Assist JSON.
         */
        void fromJson(const rapidjson::Value &val);

        /**
         * @brief Equals operator to asses whether two objects contain equivalent data.
         * 
         * @param compare second object to compare current object with.
         * @return true if both object contain equivalent data.
         * @return false if not.
         */
        bool operator==(const connection_maneuver_assist &other) const;
         /**
         * @brief Returns the inverse of equals operator.
         * 
         * @param other second object to compare current object with.
         * @return true if both objects do not contain equivalent data.
         * @return false if both objects do contain equivalent data.
         */
        bool operator!=(const connection_maneuver_assist &other) const;

    };

}