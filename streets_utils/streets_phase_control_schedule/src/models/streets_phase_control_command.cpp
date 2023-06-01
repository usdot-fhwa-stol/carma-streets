#include "streets_phase_control_command.h"
#include "streets_phase_control_schedule_exception.h"

namespace streets_phase_control_schedule
{
    streets_phase_control_command::streets_phase_control_command(/* args */)
    {
    }

    bool streets_phase_control_command::set_command_type(const std::string& command_type_str)
    {
        if (command_type_str.compare("call_veh"))
        {
        }
        else if (command_type_str.compare("call_ped"))
        {
        }
        else if (command_type_str.compare("forceoff"))
        {
        }
        else if (command_type_str.compare("hold"))
        {
        }
        else if (command_type_str.compare("omit_veh"))
        {
        }
        else if (command_type_str.compare("omit_ped"))
        {
        }
        else
        {
            throw streets_phase_control_schedule::streets_phase_control_schedule_exception("Invalid command type");
        }
    }

    streets_phase_control_command::~streets_phase_control_command()
    {
    }

}