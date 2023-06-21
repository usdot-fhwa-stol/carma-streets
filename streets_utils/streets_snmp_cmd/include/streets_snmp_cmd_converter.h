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
         * @param streets_phase_control_command a structure of phase control command.
         * @param time_cmd_m Two dimension map with key of snmp_start_time, and value of inner map. Inner map is with key of phase control schedule command type, and value of a vector of phases numbers.
         * @param pcs_cmd phase control command structure that has the command start and end time, command phase and type
         */
        void add_phase_control_schedule_command_to_two_dimension_map(uint64_t start_time, streets_phase_control_schedule::streets_phase_control_command pcs_cmd, std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> &time_cmd_m) const;
        /***
         * @brief Private method to print the two dimension map with command start time, command type and a vector of phases.
         * @param time_cmd_m a map of command type and the phases the command is applied to.
         * @param is_cmd_start indicator whether the map of command type and phases is command sent at start time or end time from the phase control schedule.
         */
        void print_two_dimension_map(std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> &time_cmd_m, bool is_cmd_start = true) const;
        /**
         * @brief Private method to perform bitwise shift or operation on input value and left shift the value by the phases number of positions.
         * @param val The value sent by the SNMP command to traffic signal controller.
         * @param phases The numbers of left shift positions applied to the value.
         */
        uint8_t bitwise_or_phases(uint8_t val, const std::vector<int>& phases) const;
        /**
         * @brief Private method to perform bitwise shift xor operation on input value and left shift the value by the phases number of positions.
         * @param val The value sent by the SNMP command to traffic signal controller.
         * @param phases The numbers of left shift positions applied to the value.
         */
        uint8_t bitwise_xor_phases(uint8_t val, const std::vector<int>& phases) const;
        /**
         * @brief Private method to update the queue with the input SNMP command. 
         * @param cmds_queue A queue of SNMP commands to be updated.
         * @param start_time The start execution time of the command.
         * @param command_type The type of command.
         * @param val The value of the command to be sent to TSC.
         * @param is_created_push_cmd A boolean indicator to determine whether to create the SNMP command and push it to the queue.
        */
        void push_snmp_command_to_queue(std::queue<snmp_cmd_struct>& cmds_queue, uint64_t start_time, PHASE_CONTROL_TYPE command_type, int64_t val, bool is_created_push_cmd) const;

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

        /**
         * @brief Conversion between phase control schedule command type and the SNMP command phase control type in the streets SNMP command structure.
         * @param command_type an enum of command types defined in the streets phase control schedule.
        */
        PHASE_CONTROL_TYPE to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE command_type) const;

        /**
         * @brief Create a vector of SNMP command structures of all current supported phase control types: CALL_VEH_PHASES, CALL_PED_PHASES, FORCEOFF_PHASES, HOLD_VEH_PHASES, OMIT_VEH_PHASES, OMIT_PED_PHASES.
         * All phase control OIDs are set to 0, and the commands are used to clear all the phases controls from the traffic signal.
         * @param execution_time when the SNMP commands are executed.
         * @return A vector of SNMP commands structures.
        */
        std::vector<streets_snmp_cmd::snmp_cmd_struct> create_clear_all_snmp_commands(uint64_t execution_time) const;
    };

} // namespace streets_snmp_cmd
