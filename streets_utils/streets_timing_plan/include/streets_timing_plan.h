#pragma once
#include "streets_timing_plan_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <math.h>
#include <chrono>
#include "streets_phase_control_command.h"
#include <spdlog/spdlog.h>

namespace streets_timing_plan
{

    struct streets_timing_plan
    {
        const std::string TIMING_PLAN_MSG_TYPE = "ActiveTimingPlan";
        std::string msg_type;
        int number_of_phase = 0;
        std::vector<int> phase_number_v;
        std::vector<int> pedestrian_walk_v;
        std::vector<int> pedestrian_clear_v;
        std::vector<int> min_green_v;
        std::vector<double> passage_v;
        std::vector<int> max_green_v;
        std::vector<double> yellow_change_v;
        std::vector<double> red_clear_v;
        std::vector<int> phase_ring_v;

        /**
         * @brief Deserialize Timing Plan JSON into Timing Plan object.
         *
         * @param valTiming Plan JSON.
         */
        void fromJson(const std::string &json);
        /**
         * @brief Serialize Timing Plan into JSON String
         */
        rapidjson::Document toJson() const;
    };
}