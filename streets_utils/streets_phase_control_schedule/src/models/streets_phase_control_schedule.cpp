#include "streets_phase_control_schedule.h"
#include <iostream>

namespace streets_phase_control_schedule
{
    void streets_phase_control_schedule::fromJson(const std::string &json)
    {
        rapidjson::Document doc;
        doc.Parse(json);
        if (doc.HasParseError())
        {
            throw streets_phase_control_schedule_exception("streets_phase_control_schedule message JSON is misformatted. JSON parsing failed!");
        }

        if (doc.HasMember("MsgType") && doc.FindMember("MsgType")->value.IsString())
        {
            std::string value = doc["MsgType"].GetString();
            toLowerCaseAndTrim(value);
            if (value != "schedule")
            {
                throw streets_phase_control_schedule_exception("streets_phase_control_schedule message is required MsgType property value (=" + value + ") is not a schedule!");
            }
        }
        else
        {
            throw streets_phase_control_schedule_exception("streets_phase_control_schedule message is missing required MsgType property!");
        }

        if (doc.HasMember("Schedule") && doc.FindMember("Schedule")->value.IsArray())
        {
            std::vector<streets_phase_control_command> cmd_v;
            // Schedule consists of an array of commands
            for (const auto &command_itr : doc["Schedule"].GetArray())
            {
                // Each command requires the four properties
                if (!command_itr.HasMember("commandType") || !command_itr.HasMember("commandPhase") || !command_itr.HasMember("commandStartTime") || !command_itr.HasMember("commandEndTime"))
                {
                    throw streets_phase_control_schedule_exception("streets_phase_control_schedule message is missing required commandType, commandPhase, commandStartTime or commandEndTime property!");
                }

                // Each command property has to be correct data type
                if (!command_itr.FindMember("commandType")->value.IsString() || !command_itr.FindMember("commandPhase")->value.IsInt() || !command_itr.FindMember("commandStartTime")->value.IsUint64() || !command_itr.FindMember("commandEndTime")->value.IsUint64())
                {
                    throw streets_phase_control_schedule_exception("streets_phase_control_schedule message commandType, commandPhase, commandStartTime or commandEndTime property has incorrect data type.");
                }

                std::string command_type_str = command_itr["commandType"].GetString();
                toLowerCaseAndTrim(command_type_str);
                streets_phase_control_command command(command_type_str, command_itr["commandPhase"].GetInt(), command_itr["commandStartTime"].GetUint64(), command_itr["commandEndTime"].GetUint64());
                cmd_v.push_back(command);
            }
            //Display a warning message when receiving two schedules with commands, and there are no clear schedule in between.
            if (!commands.empty())
            {
                SPDLOG_WARN("Current schedule has commands execution, but new schedule is not a clear schedule!");
            }
            // Always replacing the schedule commands with the latest new schedule.
            commands = cmd_v;
            // Make sure clear schedule indicator is false when there are commands in schedule
            is_clear_current_schedule = false;
        }
        else if (doc.HasMember("Schedule") && doc.FindMember("Schedule")->value.IsString())
        {
            std::string value = doc["Schedule"].GetString();
            toLowerCaseAndTrim(value);
            if (value == "clear")
            {
                // Clear the commands from the schedule
                commands.clear();
                is_clear_current_schedule = true;
            }
            else
            {
                throw streets_phase_control_schedule_exception("streets_phase_control_schedule message is Schedule property has invalid value (=" + value + ") !");
            }
        }
        else
        {
            throw streets_phase_control_schedule_exception("streets_phase_control_schedule message is missing required Schedule property, or schedule value is neither string nor array!");
        }
    }

    void streets_phase_control_schedule::toLowerCaseAndTrim(std::string &value_str) const
    {
        boost::to_lower(value_str);
        boost::algorithm::trim(value_str);
    }

    std::ostream &operator<<(std::ostream &os, const streets_phase_control_schedule& schedule)
    {
        os << "Clear status: " << (schedule.is_clear_current_schedule ? "True" : "False");
        if (!schedule.is_clear_current_schedule)
        {
            os << ", commands [";
            for (const auto &command : schedule.commands)
            {
                os << "{" << command << "},";
            }
            os << "]";
        }
        return os;
    }

}