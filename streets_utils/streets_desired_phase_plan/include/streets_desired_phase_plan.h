#pragma once
#include "streets_desired_phase_plan_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <math.h>
#include <chrono>

namespace streets_desired_phase_plan
{
    /**
     * @brief Start and end time for traffic signal green phase state assigned to a set of signal groups.
     * The start time and end time are unix timestamps in unit of milliseconds
     */
    struct signal_group2green_phase_timing
    {
        uint64_t start_time = 0;
        uint64_t end_time = 0;
        std::vector<int> signal_groups;

        inline bool operator==(const signal_group2green_phase_timing& obj2) const
        {
            return start_time == obj2.start_time && end_time == obj2.end_time && signal_groups == obj2.signal_groups;
        }
    };

    struct streets_desired_phase_plan
    {
        /**
         * @brief Unix timestamp when this desired phase plan is generated.
         */
        uint64_t timestamp = 0;
        /**
         * @brief Descriptive name for this collection.
         */
        std::vector<signal_group2green_phase_timing> desired_phase_plan;

        /**
         * @brief Serialize Desired phase plan object to rapidjson::Value for writing as JSON string
         *
         * @return rapidjson::Value serialize Desired phase plan object
         */
        std::string toJson() const;
        /**
         * @brief Deserialize Desired phase plan JSON into Desired phase plan object.
         *
         * @param val Desired phase plan JSON.
         */
        void fromJson(const std::string &json);
    };

}