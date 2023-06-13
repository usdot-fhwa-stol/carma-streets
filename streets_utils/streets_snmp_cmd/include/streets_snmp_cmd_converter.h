#pragma once
#include "streets_snmp_cmd.h"
#include "streets_phase_control_schedule.h"
#include "streets_snmp_cmd_exception.h"

namespace streets_snmp_cmd
{
    class streets_snmp_cmd_converter
    {
    public:
        streets_snmp_cmd_converter() = default;
        ~streets_snmp_cmd_converter() = default;
        /**
         * @brief Method to create SNMP commands for provided phases and a given phase control type.
         * @param phases A list of phases the SNMP command applies to.
         * @param phase_control_type The type of phase control  of the SNMP command.
         * @param start_time Time at which the snmp command needs to be sent.
         * **/
        snmp_cmd_struct create_snmp_command(const std::vector<int> &phases, PHASE_CONTROL_TYPE phase_control_type, int64_t start_time) const;
        /***
         * @brief Method to create SNMP reset commands for all phases and a given phase control type.         * 
         * @param phase_control_type The type of phase control  of the SNMP command.
         * @param start_time Time at which the snmp command needs to be sent.
        */
        snmp_cmd_struct create_snmp_reset_command(PHASE_CONTROL_TYPE phase_control_type, int64_t start_time) const;
        /**
         * @brief Method to create SNMP commands for provided phase control schedule.
         * @param phase_control_schedule The phase control schedule has a list of commands (commandPhase, commandType, commandStartTime, and commandEndTime).
        */
        std::vector<snmp_cmd_struct> create_snmp_cmds_by_phase_control_schedule(const std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule) const;

        PHASE_CONTROL_TYPE to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE command_type) const;
    };

} // namespace streets_snmp_cmd
