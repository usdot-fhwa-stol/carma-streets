#pragma once
#define RAPIDJSON_HAS_STDSTRING 1

#include "signal_phase_and_timing_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

#include <spdlog/spdlog.h>


namespace signal_phase_and_timing{
    struct time_change_details{
        /**
         * @brief When this phase first started. (tenths of a second in current or next hour)
         */
        uint64_t start_time = 0;
        /**
         * @brief Expected shortest end time. (tenths of a second in current or next hour)
         */
        uint64_t min_end_time = 0;
        /**
         * @brief Expected longest end time.  (tenths of a second in current or next hour)
         */
        uint64_t max_end_time = 0;
        /**
         * @brief Best predicted value based on other data (tenths of a second in current or next hour)
         */
        uint64_t likely_time = 0;
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

        bool operator==(const time_change_details &other) const;

        bool operator!=(const time_change_details &other) const;
    };

}