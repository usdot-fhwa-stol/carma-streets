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

    std::vector<snmp_cmd_struct> streets_snmp_cmd_converter::create_snmp_cmds_by_phase_control_schedule(const std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule) const
    {
        std::vector<snmp_cmd_struct> cmds_result;
        std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> start_time_cmd_m;
        std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> end_time_cmd_m;
        for (auto pcs_cmd : phase_control_schedule->commands)
        {
            // Start time commands
            add_phase_control_schedule_command_to_two_dimension_map(pcs_cmd.command_start_time, pcs_cmd, start_time_cmd_m);
            // End time commands
            add_phase_control_schedule_command_to_two_dimension_map(pcs_cmd.command_end_time, pcs_cmd, end_time_cmd_m);
        }
        print_two_dimension_map(start_time_cmd_m);
        print_two_dimension_map(end_time_cmd_m);

        uint64_t snmp_cmd_start_time = 0; 
        uint64_t prev_snmp_start_time = 0;
        uint8_t set_val_hold = 0;     // Initialize hold value to 00000000
        uint8_t set_val_forceoff = 0; // Initialize forceoff value to 00000000
        uint8_t set_val_call_veh = 0; // Initialize call vehicle value to 00000000
        uint8_t set_val_call_ped = 0; // Initialize call pedestrian value to 00000000
        uint8_t set_val_omit_veh = 0; // Initialize omit vehicle value to 00000000
        uint8_t set_val_omit_ped = 0; // Initialize omit pedestrian value to 00000000
        for (auto start_time_itr = start_time_cmd_m.begin(); start_time_itr != start_time_cmd_m.end(); start_time_itr++)
        {
            // Indicator to note which phase control types are received at a the start time
            bool is_hold = false;
            bool is_forceoff = false;
            bool is_call_veh = false;
            bool is_call_ped = false;
            bool is_omit_veh = false;
            bool is_omit_ped = false;
            snmp_cmd_start_time = start_time_itr->first;

            // Checking end time for commands and make sure to reset the phases if end time is earlier than start time and is older then previous start time
            for (auto end_time_itr = end_time_cmd_m.begin(); end_time_itr != end_time_cmd_m.end(); end_time_itr++)
            {
                // Command end time is earlier than current command start time and the end time is older than previous start time
                if (end_time_itr->first < start_time_itr->first && end_time_itr->first > prev_snmp_start_time)
                {
                    // Loop through each control type and find all phases belong to the control type. Bitwise xor operation on the phases for the end time phases and previous start time phases
                    for (auto inner_itr = end_time_itr->second.begin(); inner_itr != end_time_itr->second.end(); inner_itr++)
                    {
                        if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                        {
                            set_val_call_ped = bitwise_xor_phases(set_val_call_ped, inner_itr->second);
                            snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::CALL_PED_PHASES, static_cast<int64_t>(set_val_call_ped));
                            cmds_result.push_back(command);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                        {
                            set_val_call_veh = bitwise_xor_phases(set_val_call_veh, inner_itr->second);
                            snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::CALL_VEH_PHASES, static_cast<int64_t>(set_val_call_veh));
                            cmds_result.push_back(command);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                        {
                            set_val_omit_ped = bitwise_xor_phases(set_val_omit_ped, inner_itr->second);
                            snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::OMIT_PED_PHASES, static_cast<int64_t>(set_val_omit_ped));
                            cmds_result.push_back(command);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                        {
                            set_val_omit_veh = bitwise_xor_phases(set_val_omit_veh, inner_itr->second);
                            snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, static_cast<int64_t>(set_val_omit_veh));
                            cmds_result.push_back(command);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                        {
                            set_val_forceoff = bitwise_xor_phases(set_val_forceoff, inner_itr->second);
                            snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::FORCEOFF_PHASES, static_cast<int64_t>(set_val_forceoff));
                            cmds_result.push_back(command);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                        {
                            set_val_hold = bitwise_xor_phases(set_val_hold, inner_itr->second);
                            snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, static_cast<int64_t>(set_val_hold));
                            cmds_result.push_back(command);
                        }
                    }
                }
            }

            // Check start time commands, loop through each control type to find the phases. Bitwise or operation on the phases for the current start time and current control type
            for (auto inner_itr = start_time_itr->second.begin(); inner_itr != start_time_itr->second.end(); inner_itr++)
            {
                if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                {
                    set_val_call_ped = bitwise_or_phases(set_val_call_ped, inner_itr->second);
                    is_call_ped = true;
                }
                else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                {
                    set_val_call_veh = bitwise_or_phases(set_val_call_veh, inner_itr->second);
                    is_call_veh = true;
                }
                else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                {
                    set_val_omit_ped = bitwise_or_phases(set_val_omit_ped, inner_itr->second);
                    is_omit_ped = true;
                }
                else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                {
                    set_val_omit_veh = bitwise_or_phases(set_val_omit_veh, inner_itr->second);
                    is_omit_veh = true;
                }
                else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                {
                    set_val_forceoff = bitwise_or_phases(set_val_forceoff, inner_itr->second);
                    is_forceoff = true;
                }
                else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    set_val_hold = bitwise_or_phases(set_val_hold, inner_itr->second);
                    is_hold = true;
                }
            }

            // Checking end time for commands and make sure to reset the phases if end time equals to start time
            for (auto end_time_itr = end_time_cmd_m.begin(); end_time_itr != end_time_cmd_m.end(); end_time_itr++)
            {
                // Command end time equals to current command start time
                if (end_time_itr->first == start_time_itr->first)
                {
                    for (auto inner_itr = end_time_itr->second.begin(); inner_itr != end_time_itr->second.end(); inner_itr++)
                    {
                        if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                        {
                            set_val_call_ped = bitwise_xor_phases(set_val_call_ped, inner_itr->second);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                        {
                            set_val_call_veh = bitwise_xor_phases(set_val_call_veh, inner_itr->second);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                        {
                            set_val_omit_ped = bitwise_xor_phases(set_val_omit_ped, inner_itr->second);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                        {
                            set_val_omit_veh = bitwise_xor_phases(set_val_omit_veh, inner_itr->second);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                        {
                            set_val_forceoff = bitwise_xor_phases(set_val_forceoff, inner_itr->second);
                        }
                        else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                        {
                            set_val_hold = bitwise_xor_phases(set_val_hold, inner_itr->second);
                        }
                    }
                }
            }

            if (is_hold)
            {
                snmp_cmd_struct command(snmp_cmd_start_time, PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, static_cast<int64_t>(set_val_hold));
                cmds_result.push_back(command);
            }

            if (is_forceoff)
            {
                snmp_cmd_struct command(snmp_cmd_start_time, PHASE_CONTROL_TYPE::FORCEOFF_PHASES, static_cast<int64_t>(set_val_forceoff));
                cmds_result.push_back(command);
            }

            if (is_omit_veh)
            {

                snmp_cmd_struct command(snmp_cmd_start_time, PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, static_cast<int64_t>(set_val_omit_veh));
                cmds_result.push_back(command);
            }

            if (is_omit_ped)
            {
                snmp_cmd_struct command(snmp_cmd_start_time, PHASE_CONTROL_TYPE::OMIT_PED_PHASES, static_cast<int64_t>(set_val_omit_ped));
                cmds_result.push_back(command);
            }

            if (is_call_ped)
            {
                snmp_cmd_struct command(snmp_cmd_start_time, PHASE_CONTROL_TYPE::CALL_PED_PHASES, static_cast<int64_t>(set_val_call_ped));
                cmds_result.push_back(command);
            }

            if (is_call_veh)
            {
                snmp_cmd_struct command(snmp_cmd_start_time, PHASE_CONTROL_TYPE::CALL_VEH_PHASES, static_cast<int64_t>(set_val_call_veh));
                cmds_result.push_back(command);
            }

            // Update previous start time with current start time
            prev_snmp_start_time = snmp_cmd_start_time;
        } // END Start Time commands

        // Checking end time for commands and make sure to reset the phases if end time greater than the maximum (last) start time
        for (auto end_time_itr = end_time_cmd_m.begin(); end_time_itr != end_time_cmd_m.end(); end_time_itr++)
        {
            if (end_time_itr->first > snmp_cmd_start_time)
            {
                // Check end time commands, loop through each control type to find the phases. Bitwise xor operation on the phases for the end time (which is greater than maximum start time) and current control type
                for (auto inner_itr = end_time_itr->second.begin(); inner_itr != end_time_itr->second.end(); inner_itr++)
                {
                    if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                    {
                        set_val_call_ped = bitwise_xor_phases(set_val_call_ped, inner_itr->second);
                        snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::CALL_PED_PHASES, static_cast<int64_t>(set_val_call_ped));
                        cmds_result.push_back(command);
                    }
                    else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                    {
                        set_val_call_veh = bitwise_xor_phases(set_val_call_veh, inner_itr->second);
                        snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::CALL_VEH_PHASES, static_cast<int64_t>(set_val_call_veh));
                        cmds_result.push_back(command);
                    }
                    else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                    {
                        set_val_omit_ped = bitwise_xor_phases(set_val_omit_ped, inner_itr->second);
                        snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::OMIT_PED_PHASES, static_cast<int64_t>(set_val_omit_ped));
                        cmds_result.push_back(command);
                    }
                    else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                    {
                        set_val_omit_veh = bitwise_xor_phases(set_val_omit_veh, inner_itr->second);
                        snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, static_cast<int64_t>(set_val_omit_veh));
                        cmds_result.push_back(command);
                    }
                    else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                    {
                        set_val_forceoff = bitwise_xor_phases(set_val_forceoff, inner_itr->second);
                        snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::FORCEOFF_PHASES, static_cast<int64_t>(set_val_forceoff));
                        cmds_result.push_back(command);
                    }
                    else if (to_phase_control_type(inner_itr->first) == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                    {
                        set_val_hold = bitwise_xor_phases(set_val_hold, inner_itr->second);
                        snmp_cmd_struct command(end_time_itr->first, PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, static_cast<int64_t>(set_val_hold));
                        cmds_result.push_back(command);
                    }
                }
            }
        }

        return cmds_result;
    }

    uint8_t streets_snmp_cmd_converter::bitwise_or_phases(uint8_t val, std::vector<int> phases) const
    {
        for (auto phase : phases)
        {
            val |= (1 << (phase - 1));
        }
        return val;
    }

    uint8_t streets_snmp_cmd_converter::bitwise_xor_phases(uint8_t val, std::vector<int> phases) const
    {
        for (auto phase : phases)
        {
            val ^= (1 << (phase - 1));
        }
        return val;
    }

    void streets_snmp_cmd_converter::add_phase_control_schedule_command_to_two_dimension_map(uint64_t start_time, streets_phase_control_schedule::streets_phase_control_command pcs_cmd, std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> &time_cmd_m) const
    {
        if (time_cmd_m.find(start_time) == time_cmd_m.end())
        {
            std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>> cmd_type_phase_m;
            std::vector<int> phases;
            phases.push_back(pcs_cmd.command_phase);
            cmd_type_phase_m.insert(std::make_pair(pcs_cmd.command_type, phases));
            time_cmd_m.insert(std::make_pair(start_time, cmd_type_phase_m));
        }
        else
        {
            auto tmp_cmd_phase_m = time_cmd_m.find(start_time)->second;
            // Different command types at the same start time
            if (tmp_cmd_phase_m.find(pcs_cmd.command_type) == tmp_cmd_phase_m.end())
            {
                std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>> cmd_type_phase_m;
                std::vector<int> phases;
                phases.push_back(pcs_cmd.command_phase);
                time_cmd_m.find(start_time)->second.insert(std::make_pair(pcs_cmd.command_type, phases));
            }
            else
            {
                // Same command type at the same start time
                time_cmd_m.find(start_time)->second.find(pcs_cmd.command_type)->second.push_back(pcs_cmd.command_phase);
            }
        }
    }

    void streets_snmp_cmd_converter::print_two_dimension_map(std::map<uint64_t, std::map<streets_phase_control_schedule::COMMAND_TYPE, std::vector<int>>> &time_cmd_m) const
    {
        SPDLOG_INFO("---------------Printing Two Dimension Command Map---------");
        for (auto cmd : time_cmd_m)
        {
            SPDLOG_INFO("Start Timestamp: {0}", cmd.first);
            for (auto cmd_inner : cmd.second)
            {
                streets_phase_control_schedule::streets_phase_control_command cmd_tmp;
                std::string command_type_str = cmd_tmp.COMMAND_TYPE_to_string(cmd_inner.first);
                SPDLOG_INFO("\tCommand Type: {0}", command_type_str);
                std::string cmd_inner_inner_str = "";
                for (auto cmd_inner_inner : cmd_inner.second)
                {
                    cmd_inner_inner_str += std::to_string(cmd_inner_inner) + ",";
                }
                SPDLOG_INFO("\t\tPhases: {0}", cmd_inner_inner_str);
            }
            SPDLOG_INFO("\n");
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
} // namespace streets_snmp_cmd
