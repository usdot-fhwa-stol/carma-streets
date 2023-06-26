#include "streets_snmp_cmd_converter.h"
#include <map>

namespace streets_snmp_cmd
{
    snmp_cmd_struct streets_snmp_cmd_converter::create_snmp_command_by_phases(const std::vector<int> &phases, PHASE_CONTROL_TYPE phase_control_type, int64_t start_time) const
    {
        uint8_t set_val = 0; // Initialize to 00000000
        set_val = bitwise_or_phases(set_val, phases);
        snmp_cmd_struct command(start_time, phase_control_type, static_cast<int64_t>(set_val));
        return command;
    }

    snmp_cmd_struct streets_snmp_cmd_converter::create_snmp_reset_command(PHASE_CONTROL_TYPE phase_control_type, int64_t start_time) const
    {
        snmp_cmd_struct command(start_time, phase_control_type, static_cast<int64_t>(0));
        return command;
    }

    std::queue<snmp_cmd_struct> streets_snmp_cmd_converter::create_snmp_cmds_by_phase_control_schedule(const std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule) const
    {
        // A queue of SNMP commands to return after processing the input phase control schedule.
        std::queue<snmp_cmd_struct> cmds_result;
        std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> start_time_cmd_m;
        std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> end_time_cmd_m;
        for (auto &pcs_cmd : phase_control_schedule->commands)
        {
            // Start time commands
            add_phase_control_schedule_command_to_two_dimension_map(pcs_cmd.command_start_time, pcs_cmd, start_time_cmd_m);
            // End time commands
            add_phase_control_schedule_command_to_two_dimension_map(pcs_cmd.command_end_time, pcs_cmd, end_time_cmd_m);
        }
        print_two_dimension_map(start_time_cmd_m);
        print_two_dimension_map(end_time_cmd_m, false);

        // Keep track of the SNMP command start time. It is used by the snmp_cmd_struct to create a streets defined SNMP command.
        uint64_t snmp_cmd_start_time = 0;
        // Keep track of the previous SNMP command start time. It is used to determine whether to create a separate SNMP command based on end time.
        uint64_t prev_snmp_start_time = 0;
        // Below values are affected by the whole phase control schedule, and it is increased or decreased by all phases in the vector of phase control schedule commands.
        uint8_t set_val_hold = 0;     // Initialize hold value to 00000000
        uint8_t set_val_forceoff = 0; // Initialize forceoff value to 00000000
        uint8_t set_val_call_veh = 0; // Initialize call vehicle value to 00000000
        uint8_t set_val_call_ped = 0; // Initialize call pedestrian value to 00000000
        uint8_t set_val_omit_veh = 0; // Initialize omit vehicle value to 00000000
        uint8_t set_val_omit_ped = 0; // Initialize omit pedestrian value to 00000000
        /***
         * There are two maps (start_time_cmd_m, end_time_cmd_m) to work with in the following loops. The start time commands map contains all the commands at start time.
         * The end time commands map contains all the commands at end time.
         * 1. Interate the start time commands map.
         * ** 1.1 Iterate the end time commands map, checking if the end time command is executed before the current start time command, and after the previous start time command.
         *    (Note: This usually applies to second and forward commands from the start time commands map)
         * ** 1.2 If yes, create new SNMP commands using the end time.
         * ** Iterate the nested map of command type and phases, perform the bitwise or operation on the values using the phases.Create SNMP command for each command type and phases, and add the SNMP command to the queue.
         *
         * 2. Iterate the nested map of command type and phases. At each start time, multiple command types are expected. Each command type has multiple phases.
         *    It needs to create streets SNMP command for each command type. The boolean indicators (is_forceoff, is_hold, is_call_veh, is_call_ped, is_omit_veh, is_omit_ped) are used to indicate
         *    which types of commands should be executed at the current start time.  The set_val_hold, set_val_forceoff, set_val_call_veh, set_val_call_ped, set_val_omit_ped, set_val_omit_veh
         *    values are used to accumulate the phases associated to each command type at the current start time.
         *    Once finished interating the nested map of command types and phases. It will create the streets SNMP command based on the boolean indicator to corresponding create SNMP command and values for each command type.
         * 3. Iterate the end time commands map in case some commands ends the same time as the start time commands.
         * 4. Check the boolean indicators and create corresponding SNMP commands. Add the SNMP commands to the queue.
         *
         * 5. Iterate the end time commands map in case some commands ends after finishing all commands at the start time.
         * ** Checking the snmp_cmd_start_time and the end time from the end time command maps. If there are any commands end time that is greater than snmp_cmd_start_time, create streets
         * ** SNMP commands for that end time command, and add the SNMP commands to the queue. (Note: At the end of the step 1 iteration, the snmp_cmd_start_time is updated with the maximum start time.)
         * */
        //Step 1: Loop through start time commands map
        for (auto start_time_cmd_itr = start_time_cmd_m.begin(); start_time_cmd_itr != start_time_cmd_m.end(); start_time_cmd_itr++)
        {
            // Indicator to note which phase control types are received at a the start time
            bool is_hold = false;
            bool is_forceoff = false;
            bool is_call_veh = false;
            bool is_call_ped = false;
            bool is_omit_veh = false;
            bool is_omit_ped = false;
            snmp_cmd_start_time = start_time_cmd_itr->first;

            // Step 1.1 & 1.2: Checking end time for commands and make sure to reset the phases if end time is earlier than start time and is older then previous start time
            for (auto end_time_cmd_itr = end_time_cmd_m.begin(); end_time_cmd_itr != end_time_cmd_m.end(); end_time_cmd_itr++)
            {
                // Command end time is earlier than current command start time and the end time is older than previous start time
                if (end_time_cmd_itr->first < start_time_cmd_itr->first && end_time_cmd_itr->first > prev_snmp_start_time)
                {
                    // Loop through each control type and find all phases belong to the control type. Bitwise xor operation on the phases for the end time phases and previous start time phases
                    for (auto inner_itr = end_time_cmd_itr->second.begin(); inner_itr != end_time_cmd_itr->second.end(); inner_itr++)
                    {
                        // Phases at the end time
                        auto phases = inner_itr->second;
                        auto phase_control_type = to_phase_control_type(inner_itr->first);
                        if (phase_control_type == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                        {
                            set_val_call_ped = bitwise_xor_phases(set_val_call_ped, phases);
                            push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::CALL_PED_PHASES, static_cast<int64_t>(set_val_call_ped));
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                        {
                            set_val_call_veh = bitwise_xor_phases(set_val_call_veh, phases);
                            push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::CALL_VEH_PHASES, static_cast<int64_t>(set_val_call_veh));
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                        {
                            set_val_omit_ped = bitwise_xor_phases(set_val_omit_ped, phases);
                            push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::OMIT_PED_PHASES, static_cast<int64_t>(set_val_omit_ped));
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                        {
                            set_val_omit_veh = bitwise_xor_phases(set_val_omit_veh, phases);
                            push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, static_cast<int64_t>(set_val_omit_veh));
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                        {
                            set_val_forceoff = bitwise_xor_phases(set_val_forceoff, phases);
                            push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::FORCEOFF_PHASES, static_cast<int64_t>(set_val_forceoff));
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                        {
                            set_val_hold = bitwise_xor_phases(set_val_hold, phases);
                            push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, static_cast<int64_t>(set_val_hold));
                        }
                    }
                }
            }

            // Step 2: Check start time commands, loop through each control type to find the phases. Bitwise or operation on the phases for the current start time and current control type
            for (auto inner_itr = start_time_cmd_itr->second.begin(); inner_itr != start_time_cmd_itr->second.end(); inner_itr++)
            {
                auto phases = inner_itr->second;
                auto phase_control_type = to_phase_control_type(inner_itr->first);
                if (phase_control_type == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                {
                    set_val_call_ped = bitwise_or_phases(set_val_call_ped, phases);
                    is_call_ped = true;
                }
                else if (phase_control_type == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                {
                    set_val_call_veh = bitwise_or_phases(set_val_call_veh, phases);
                    is_call_veh = true;
                }
                else if (phase_control_type == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                {
                    set_val_omit_ped = bitwise_or_phases(set_val_omit_ped, phases);
                    is_omit_ped = true;
                }
                else if (phase_control_type == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                {
                    set_val_omit_veh = bitwise_or_phases(set_val_omit_veh, phases);
                    is_omit_veh = true;
                }
                else if (phase_control_type == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                {
                    set_val_forceoff = bitwise_or_phases(set_val_forceoff, phases);
                    is_forceoff = true;
                }
                else if (phase_control_type == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    set_val_hold = bitwise_or_phases(set_val_hold, phases);
                    is_hold = true;
                }
            }

            // Step 3: Checking end time for commands and make sure to reset the phases if end time equals to start time
            for (auto end_time_cmd_itr = end_time_cmd_m.begin(); end_time_cmd_itr != end_time_cmd_m.end(); end_time_cmd_itr++)
            {
                // Command end time equals to current command start time
                if (end_time_cmd_itr->first == start_time_cmd_itr->first)
                {
                    for (auto inner_itr = end_time_cmd_itr->second.begin(); inner_itr != end_time_cmd_itr->second.end(); inner_itr++)
                    {
                        auto phase_control_type = to_phase_control_type(inner_itr->first);
                        if (phase_control_type == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                        {
                            set_val_call_ped = bitwise_xor_phases(set_val_call_ped, inner_itr->second);
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                        {
                            set_val_call_veh = bitwise_xor_phases(set_val_call_veh, inner_itr->second);
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                        {
                            set_val_omit_ped = bitwise_xor_phases(set_val_omit_ped, inner_itr->second);
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                        {
                            set_val_omit_veh = bitwise_xor_phases(set_val_omit_veh, inner_itr->second);
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                        {
                            set_val_forceoff = bitwise_xor_phases(set_val_forceoff, inner_itr->second);
                        }
                        else if (phase_control_type == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                        {
                            set_val_hold = bitwise_xor_phases(set_val_hold, inner_itr->second);
                        }
                    }
                }
            }
            
            //Step 4:
            if(is_hold)
            {
                push_snmp_command_to_queue(cmds_result, snmp_cmd_start_time, PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, static_cast<int64_t>(set_val_hold));
            }
            if(is_forceoff)
            {
                push_snmp_command_to_queue(cmds_result, snmp_cmd_start_time, PHASE_CONTROL_TYPE::FORCEOFF_PHASES, static_cast<int64_t>(set_val_forceoff));
            }
            if(is_omit_veh)
            {
                push_snmp_command_to_queue(cmds_result, snmp_cmd_start_time, PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, static_cast<int64_t>(set_val_omit_veh));
            }
            if(is_omit_ped)
            {
                push_snmp_command_to_queue(cmds_result, snmp_cmd_start_time, PHASE_CONTROL_TYPE::OMIT_PED_PHASES, static_cast<int64_t>(set_val_omit_ped));
            }
            if(is_call_ped)
            {
                push_snmp_command_to_queue(cmds_result, snmp_cmd_start_time, PHASE_CONTROL_TYPE::CALL_PED_PHASES, static_cast<int64_t>(set_val_call_ped));
            }
            if(is_call_veh)
            {
                push_snmp_command_to_queue(cmds_result, snmp_cmd_start_time, PHASE_CONTROL_TYPE::CALL_VEH_PHASES, static_cast<int64_t>(set_val_call_veh));
            }
            // Update previous start time with current start time
            prev_snmp_start_time = snmp_cmd_start_time;
        } // END Start Time commands

        // Step 5: Checking end time for commands and make sure to reset the phases if end time greater than the maximum (last) start time
        for (auto end_time_cmd_itr = end_time_cmd_m.begin(); end_time_cmd_itr != end_time_cmd_m.end(); end_time_cmd_itr++)
        {
            if (end_time_cmd_itr->first > snmp_cmd_start_time)
            {
                // Check end time commands, loop through each control type to find the phases. Bitwise xor operation on the phases for the end time (which is greater than maximum start time) and current control type
                for (auto inner_itr = end_time_cmd_itr->second.begin(); inner_itr != end_time_cmd_itr->second.end(); inner_itr++)
                {
                    auto phase_control_type = to_phase_control_type(inner_itr->first);
                    if (phase_control_type == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                    {
                        set_val_call_ped = bitwise_xor_phases(set_val_call_ped, inner_itr->second);
                        push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::CALL_PED_PHASES, static_cast<int64_t>(set_val_call_ped));
                    }
                    else if (phase_control_type == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                    {
                        set_val_call_veh = bitwise_xor_phases(set_val_call_veh, inner_itr->second);
                        push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::CALL_VEH_PHASES, static_cast<int64_t>(set_val_call_veh));
                    }
                    else if (phase_control_type == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                    {
                        set_val_omit_ped = bitwise_xor_phases(set_val_omit_ped, inner_itr->second);
                        push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::OMIT_PED_PHASES, static_cast<int64_t>(set_val_omit_ped));
                    }
                    else if (phase_control_type == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                    {
                        set_val_omit_veh = bitwise_xor_phases(set_val_omit_veh, inner_itr->second);
                        push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, static_cast<int64_t>(set_val_omit_veh));
                    }
                    else if (phase_control_type == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                    {
                        set_val_forceoff = bitwise_xor_phases(set_val_forceoff, inner_itr->second);
                        push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::FORCEOFF_PHASES, static_cast<int64_t>(set_val_forceoff));
                    }
                    else if (phase_control_type == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                    {
                        set_val_hold = bitwise_xor_phases(set_val_hold, inner_itr->second);
                        push_snmp_command_to_queue(cmds_result, end_time_cmd_itr->first, PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, static_cast<int64_t>(set_val_hold));
                    }
                }
            }
        }//END end time commands map

        return cmds_result;
    }

    uint8_t streets_snmp_cmd_converter::bitwise_or_phases(uint8_t val, const std::vector<int> &phases) const
    {
        for (auto phase : phases)
        {
            val |= (1 << (phase - 1));
        }
        return val;
    }

    uint8_t streets_snmp_cmd_converter::bitwise_xor_phases(uint8_t val, const std::vector<int> &phases) const
    {
        for (auto phase : phases)
        {
            val ^= (1 << (phase - 1));
        }
        return val;
    }

    void streets_snmp_cmd_converter::add_phase_control_schedule_command_to_two_dimension_map(uint64_t start_time, streets_phase_control_schedule::streets_phase_control_command pcs_cmd, std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> &time_cmd_m) const
    {
        // Checking start time, if the start time as key does not exist in the map, add the start time, the phase and control type as new record in the map.
        if (time_cmd_m.find(start_time) == time_cmd_m.end())
        {
            std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>> cmd_type_phase_m;
            std::vector<int> phases;
            phases.push_back(pcs_cmd.command_phase);
            cmd_type_phase_m.try_emplace(pcs_cmd.command_type, phases);
            time_cmd_m.try_emplace(start_time, cmd_type_phase_m);
        }
        else
        {
            /***
             * Checking start time, if the start time already exist, then check the command type.
             * If it is a new command type, add the phase abd control type as new record in the nest map.
             * If the command type already exists, add the phase to the existing phases list.
             * ***/
            auto tmp_cmd_phase_m = time_cmd_m.find(start_time)->second;
            // Different command types at the same start time
            if (tmp_cmd_phase_m.find(pcs_cmd.command_type) == tmp_cmd_phase_m.end())
            {
                std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>> cmd_type_phase_m;
                std::vector<int> phases;
                phases.push_back(pcs_cmd.command_phase);
                time_cmd_m.find(start_time)->second.try_emplace(pcs_cmd.command_type, phases);
            }
            else
            {
                // Same command type at the same start time
                time_cmd_m.find(start_time)->second.find(pcs_cmd.command_type)->second.push_back(pcs_cmd.command_phase);
            }
        }
    }

    void streets_snmp_cmd_converter::print_two_dimension_map(std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> &time_cmd_m, bool is_cmd_start) const
    {
        SPDLOG_DEBUG("---------------Printing Two Dimension Command Map of Phase Control Schedule Command and Phases---------");
        for (auto &cmd : time_cmd_m)
        {
            // Start/End time
            is_cmd_start ? SPDLOG_DEBUG("Start Timestamp: {0}", cmd.first) : SPDLOG_DEBUG("End Timestamp: {0}", cmd.first);
            for (auto &cmd_inner : cmd.second)
            {
                // Command type
                streets_phase_control_schedule::streets_phase_control_command cmd_tmp;
                std::string command_type_str = cmd_tmp.COMMAND_TYPE_to_string(cmd_inner.first);
                SPDLOG_DEBUG("\tCommand Type: {0}", command_type_str);
                // Phases
                std::string cmd_inner_inner_str = "";
                for (auto &cmd_inner_inner : cmd_inner.second)
                {
                    cmd_inner_inner_str += std::to_string(cmd_inner_inner) + ",";
                }
                SPDLOG_DEBUG("\t\tPhases: {0}", cmd_inner_inner_str);
            }
            SPDLOG_DEBUG("\n");
        }
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
        case streets_phase_control_schedule::COMMAND_TYPE::HOLD_VEH_PHASES:
            result = PHASE_CONTROL_TYPE::HOLD_VEH_PHASES;
            break;
        case streets_phase_control_schedule::COMMAND_TYPE::OMIT_PED_PHASES:
            result = PHASE_CONTROL_TYPE::OMIT_PED_PHASES;
            break;
        case streets_phase_control_schedule::COMMAND_TYPE::OMIT_VEH_PHASES:
            result = PHASE_CONTROL_TYPE::OMIT_VEH_PHASES;
            break;
        default:
            throw streets_snmp_cmd_exception("Phase control schedule command type does not have a mapping SNMP phase control type.");
            break;
        }
        return result;
    }

    std::vector<streets_snmp_cmd::snmp_cmd_struct> streets_snmp_cmd_converter::create_clear_all_snmp_commands(uint64_t execution_time) const
    {
        std::vector<streets_snmp_cmd::snmp_cmd_struct> snmp_cmds_result;
        snmp_cmds_result.push_back(create_snmp_reset_command(streets_snmp_cmd::PHASE_CONTROL_TYPE::CALL_PED_PHASES, execution_time));
        snmp_cmds_result.push_back(create_snmp_reset_command(streets_snmp_cmd::PHASE_CONTROL_TYPE::CALL_VEH_PHASES, execution_time));
        snmp_cmds_result.push_back(create_snmp_reset_command(streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_PED_PHASES, execution_time));
        snmp_cmds_result.push_back(create_snmp_reset_command(streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, execution_time));
        snmp_cmds_result.push_back(create_snmp_reset_command(streets_snmp_cmd::PHASE_CONTROL_TYPE::FORCEOFF_PHASES, execution_time));
        snmp_cmds_result.push_back(create_snmp_reset_command(streets_snmp_cmd::PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, execution_time));
        return snmp_cmds_result;
    }

    void streets_snmp_cmd_converter::push_snmp_command_to_queue(std::queue<snmp_cmd_struct> &cmds_queue, uint64_t start_time, PHASE_CONTROL_TYPE command_type, int64_t val) const
    {
        snmp_cmd_struct command(start_time, command_type, val);
        cmds_queue.push(command);
    }
} // namespace streets_snmp_cmd
