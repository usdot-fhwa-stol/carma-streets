#include "streets_snmp_cmd_converter.h"

namespace streets_snmp_cmd
{
    snmp_cmd_struct streets_snmp_cmd_converter::create_snmp_command(const std::vector<int> &phases, PHASE_CONTROL_TYPE phase_control_type, int64_t start_time) const
    {
        uint8_t set_val = 0; // Initialize to 00000000

        for (auto phase : phases)
        {
            set_val |= (1 << (phase - 1));
        }

        snmp_cmd_struct command(start_time, phase_control_type, static_cast<int64_t>(set_val));
        return command;
    }

    std::vector<snmp_cmd_struct> streets_snmp_cmd_converter::create_snmp_cmds_by_phase_control_schedule(const std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule) const
    {
        std::vector<snmp_cmd_struct> cmds_result;
        for (auto pcs_cmd : phase_control_schedule->commands)
        {
            uint8_t set_val = 0; // Initialize to 00000000
            snmp_cmd_struct end_cmd(pcs_cmd.command_end_time, to_phase_control_type(pcs_cmd.command_type), static_cast<int64_t>(set_val));
            cmds_result.push_back(end_cmd);

            set_val |= (1 << (pcs_cmd.command_phase % 8 - 1));
            snmp_cmd_struct start_cmd(pcs_cmd.command_start_time, to_phase_control_type(pcs_cmd.command_type), static_cast<int64_t>(set_val));
            cmds_result.push_back(start_cmd);
        }
        return cmds_result;
    }

    PHASE_CONTROL_TYPE streets_snmp_cmd_converter::to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE command_type) const
    {
        PHASE_CONTROL_TYPE result;
        switch (command_type)
        {
        case streets_phase_control_schedule::COMMAND_TYPE::CALL_PED_PHASES:
            result = PHASE_CONTROL_TYPE::CALL_PED_PHASES;
            break;
        case streets_phase_control_schedule::COMMAND_TYPE::CALL_VEH_PHASES:
            result = PHASE_CONTROL_TYPE::CALL_VEH_PHASES;
            break;
        case streets_phase_control_schedule::COMMAND_TYPE::FORCEOFF_PHASES:
            result = PHASE_CONTROL_TYPE::FORCEOFF_PHASES;
            break;
        case streets_phase_control_schedule::COMMAND_TYPE::HOLD_VEH_PHASES :
            result = PHASE_CONTROL_TYPE::HOLD_VEH_PHASES;
            break;
        case streets_phase_control_schedule::COMMAND_TYPE::OMIT_PED_PHASES :
            result = PHASE_CONTROL_TYPE::OMIT_PED_PHASES;
            break;
        case streets_phase_control_schedule::COMMAND_TYPE::OMIT_VEH_PHASES :
            result = PHASE_CONTROL_TYPE::OMIT_VEH_PHASES;
            break;
        default:
            throw streets_snmp_cmd_exception("Phase control schedule command type does not have a mapping SNMP phase control type.");
            break;
        }
        return result;
    }
} // namespace streets_snmp_cmd
