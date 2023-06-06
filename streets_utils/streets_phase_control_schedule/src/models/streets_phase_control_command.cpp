#include "streets_phase_control_command.h"
#include "streets_phase_control_schedule_exception.h"

namespace streets_phase_control_schedule
{
    streets_phase_control_command::streets_phase_control_command(const std::string &command_type_str, int phase, uint64_t start_time, uint64_t end_time)
        : command_phase(phase), command_start_time(start_time), command_end_time(end_time)
    {
        set_command_type(command_type_str);
    }

    void streets_phase_control_command::set_command_type(const std::string &command_type_str)
    {
        if (command_type_str == "call_veh")
        {
            command_type = COMMAND_TYPE::CALL_VEH_PHASES;
        }
        else if (command_type_str == "call_ped")
        {
            command_type = COMMAND_TYPE::CALL_PED_PHASES;
        }
        else if (command_type_str == "forceoff")
        {
            command_type = COMMAND_TYPE::FORCEOFF_PHASES;
        }
        else if (command_type_str == "hold")
        {
            command_type = COMMAND_TYPE::HOLD_VEH_PHASES;
        }
        else if (command_type_str == "omit_veh")
        {
            command_type = COMMAND_TYPE::OMIT_VEH_PHASES;
        }
        else if (command_type_str == "omit_ped")
        {
            command_type = COMMAND_TYPE::OMIT_PED_PHASES;
        }
        else
        {
            throw streets_phase_control_schedule::streets_phase_control_schedule_exception("Invalid command type");
        }
    }

    std::ostream &operator<<(std::ostream &os, const streets_phase_control_command command)
    {
        std::string command_type_str;
        switch (command.command_type)
        {
        case COMMAND_TYPE::CALL_VEH_PHASES:
            command_type_str = "call_veh";
            break;
        case COMMAND_TYPE::CALL_PED_PHASES:
            command_type_str = "call_ped";
            break;
        case COMMAND_TYPE::FORCEOFF_PHASES:
            command_type_str = "forceoff";
            break;
        case COMMAND_TYPE::HOLD_VEH_PHASES:
            command_type_str = "hold";
            break;
        case COMMAND_TYPE::OMIT_VEH_PHASES:
            command_type_str = "omit_veh";
            break;
        case COMMAND_TYPE::OMIT_PED_PHASES:
            command_type_str = "omit_ped";
            break;
        default:
            break;
        }
        os << "Command type: " << command_type_str << ", command phase: " << command.command_phase << ", start time: " << command.command_start_time << ", end time: " << std::setprecision(17)<< command.command_end_time;
        return os;
    }

}