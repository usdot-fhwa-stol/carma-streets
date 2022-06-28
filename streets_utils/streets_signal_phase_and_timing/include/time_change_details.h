#pragma once

#include "signal_phase_and_timing_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <spdlog/spdlog.h>


namespace signal_phase_and_timing{
    struct time_change_details{
        /**
         * @brief When this phase first started. (tenths of a second in current or next hour)
         */
        uint16_t start_time = 36001;
        /**
         * @brief Expected shortest end time. (tenths of a second in current or next hour)
         */
        uint16_t min_end_time = 36001;
        /**
         * @brief Expected longest end time.  (tenths of a second in current or next hour)
         */
        uint16_t max_end_time = 36001;
        /**
         * @brief Best predicted value based on other data (tenths of a second in current or next hour)
         */
        uint16_t likely_time = 36001;
        /**
         * @brief Applies to the above time interval only
         * -- Value   Probability
         * --   0         21%
         * --   1         36%
         * --   2         47%
         * --   3         56%
         * --   4         62%
         * --   5         68%
         * --   6         73%
         * --   7         77%
         * --   8         81%
         * --   9         85%
         * --   10        88%
         * --   11        91%
         * --   12        94%
         * --   13        96%
         * --   14        98%
         * --   15        100%
         */
        uint8_t confidence = 0;

        /**
         * @brief A rough estimate of time when this phase may next occur. (tenths of a second in current or next hour)
         */
        uint16_t next_time = 36001;
        /**
         * @brief Serialize Time Change Details object to rapidjson::Value for writing as JSON string
         * 
         * @return rapidjson::Value serialize Time Change Details object
         */
        rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const;
        /**
         * @brief Deserialize Time Change Details JSON into Time Change Details object.
         * 
         * @param val Time Change Details JSON.
         */
        void fromJson(const rapidjson::Value &val);
        /**
         * @brief Equals operator to asses whether two objects contain equivalent data.
         * 
         * @param compare second object to compare current object with.
         * @return true if both object contain equivalent data.
         * @return false if not.
         */
        bool operator==(const time_change_details &other) const;
        /**
         * @brief Returns the inverse of equals operator.
         * 
         * @param other second object to compare current object with.
         * @return true if both objects do not contain equivalent data.
         * @return false if both objects do contain equivalent data.
         */
        bool operator!=(const time_change_details &other) const;
    };

}