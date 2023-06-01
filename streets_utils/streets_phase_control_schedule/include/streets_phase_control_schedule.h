#pragma once
#include "streets_phase_control_schedule_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <math.h>
#include <chrono>

namespace streets_phase_control_schedule
{
    
    struct streets_phase_control_schedule
    {
        /**
         * @brief Deserialize Phase control plan JSON into Phase control plan object.
         *
         * @param val Phase control plan JSON.
         */
        void fromJson(const std::string &json);
    };

}