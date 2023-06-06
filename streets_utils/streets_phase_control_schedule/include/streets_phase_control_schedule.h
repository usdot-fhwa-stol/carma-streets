#pragma once
#include "streets_phase_control_schedule_exception.h"
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <math.h>
#include <chrono>
#include "streets_phase_control_command.h"

namespace streets_phase_control_schedule
{

    struct streets_phase_control_schedule
    {
        // When a new schedule with commands is received, the commands list is populated for schedule execution.
        std::vector<streets_phase_control_command> commands;

        /***
         * By default the indicator is set to false assumming that the initial phase control schedule does not have clear execution schedule.
         * When a clear execution schedule is received, the indicator should be set to true to indicate clearing all scheduled jobs from the current phase control schedule.
         * Note: A clear execution schedule has an empty list of commands. However, A schedule with an empty list of commands does not mean it is a clear schedule. Intially the schedule object has an empty list of commands. 
         * */
        bool is_clear_current_schedule = false;

        /**
         * @brief Deserialize Phase control plan JSON into Phase control plan object.
         *
         * @param val Phase control plan JSON.
         */
        void fromJson(const std::string &json);
        /**
         * @brief Transform the input value to lower case and trim the leading and tailing spaces
         * @param string input value reference
         */
        void toLowerCaseAndTrim(std::string &value_str);

        // Overload operator<< to print schedule
        friend std::ostream &operator<<(std::ostream &os, const streets_phase_control_schedule schedule);
    };
}