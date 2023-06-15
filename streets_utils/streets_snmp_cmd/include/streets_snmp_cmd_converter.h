#pragma once
#include "streets_snmp_cmd.h"
#include "streets_phase_control_schedule.h"
#include "streets_snmp_cmd_exception.h"
#include <map>
#include <queue>

namespace streets_snmp_cmd
{
    class streets_snmp_cmd_converter
    {
    private:
        /***
         * @brief Private method to support the phase control schedule conversion to snmp_cmd_struct. It takes each individual streets_phase_control_command, and populate a two dimension map with start time, command type and  a vector of phases.
         * Sort the streets_phase_control_command using start time, and categorize the phases based on the phase control schedule command types.
         * @param streets_phase_control_command
         * @param map Two dimension map with key of snmp_start_time, and value of inner map. Inner map is with key of phase control schedule command type, and value of a vector of phases numbers.
         */
        void add_phase_control_schedule_command_to_two_dimension_map(uint64_t start_time, streets_phase_control_schedule::streets_phase_control_command pcs_cmd, std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> &time_cmd_m) const;
        /***
         * @brief Private method to print the two dimension map with command start time, command type and a vector of phases.
         */
        void print_two_dimension_map(std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> &time_cmd_m, bool is_cmd_start = true) const;
        /**
         * @brief Private method to performe bitwise shift or operation on input value and left shift the value by the phases number of positions.
         */
        uint8_t bitwise_or_phases(uint8_t val, std::vector<int> phases) const;
        /**
         * @brief Private method to performe bitwise shift xor operation on input value and left shift the value by the phases number of positions.
         */
        uint8_t bitwise_xor_phases(uint8_t val, std::vector<int> phases) const;

    public:
        streets_snmp_cmd_converter() = default;
        ~streets_snmp_cmd_converter() = default;
        /**
         * @brief Method to create SNMP commands for provided phases and a given phase control type.
         * @param phases A list of phases the SNMP command applies to.
         * @param phase_control_type The type of phase control  of the SNMP command.
         * @param start_time Time at which the snmp command needs to be sent.
         * **/
        snmp_cmd_struct create_snmp_command_by_phases(const std::vector<int> &phases, PHASE_CONTROL_TYPE phase_control_type, int64_t start_time) const;
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
        std::queue<snmp_cmd_struct> create_snmp_cmds_by_phase_control_schedule(const std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule) const;

        PHASE_CONTROL_TYPE to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE command_type) const;
    };

} // namespace streets_snmp_cmd
