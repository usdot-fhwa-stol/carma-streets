#include "streets_snmp_cmd.h"

namespace streets_snmp_cmd
{
    std::string snmp_cmd_struct::get_cmd_info() const
    {

        std::string control_type_str;
        switch (control_type_)
        {
        case PHASE_CONTROL_TYPE::HOLD_VEH_PHASES:
            control_type_str = "Vehicle Hold";
            break;
        case PHASE_CONTROL_TYPE::FORCEOFF_PHASES:
            control_type_str = "Vehicle Forceoff";
            break;
        case PHASE_CONTROL_TYPE::CALL_PED_PHASES:
            control_type_str = "Pedestrian Call";
            break;
        case PHASE_CONTROL_TYPE::CALL_VEH_PHASES:
            control_type_str = "Vehicle Call";
            break;
        case PHASE_CONTROL_TYPE::OMIT_PED_PHASES:
            control_type_str = "Pedestrian Omit";
            break;
        case PHASE_CONTROL_TYPE::OMIT_VEH_PHASES:
            control_type_str = "Vehicle Omit";
            break;
        default:
            break;
        }
        std::string execution_start_time = std::to_string(start_time_);

        // Convert value set to phases nums
        std::string value_set = std::to_string(set_val_.val_int);

        return "control_cmd_type:" + control_type_str + "; execution_start_time:" + execution_start_time + "; value_set:" + value_set;
    }
}