#include "control_tsc_state.h"
#include <chrono>

namespace traffic_signal_controller_service
{
    control_tsc_state::control_tsc_state(std::shared_ptr<snmp_client> snmp_client, 
        std::shared_ptr<tsc_state> _tsc_state)  
                            : snmp_client_worker_(snmp_client), _tsc_state(_tsc_state)
    {
                
    }

    void control_tsc_state::update_tsc_control_queue(std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan,
                                             std::queue<snmp_cmd_struct>& tsc_command_queue) const
    {
        if(desired_phase_plan->desired_phase_plan.empty()){
            SPDLOG_DEBUG("No events in desired phase plan");
            return;
        }
        if(desired_phase_plan->desired_phase_plan.size() == 1){
            SPDLOG_DEBUG("TSC service assumes first event is already set, no update to queue required");
            return;
        }

        // Check if desired phase plan is valid
        // Check no repeated signal groups in adjacent events
        for(int i = 0; i < desired_phase_plan->desired_phase_plan.size() - 1; ++i){
            
            auto event = desired_phase_plan->desired_phase_plan[0];
            auto next_event = desired_phase_plan->desired_phase_plan[1];

            for(auto signal_group : event.signal_groups)
            {
                auto it = std::find(next_event.signal_groups.begin(), next_event.signal_groups.end(), signal_group);
                if(it != next_event.signal_groups.end())
                {
                    SPDLOG_ERROR("TSC Service assumes adjacent events dont have the same signal_group. Given Desired phase plan fails this assumption");
                    throw control_tsc_state_exception("Repeating signal group found in adjacent events. Desired phase plan cannot be set");
                }
            }
        }
        

        //Reset queue
        tsc_command_queue = std::queue<snmp_cmd_struct>();

        // add Omit and Hold commands
        auto first_event = desired_phase_plan->desired_phase_plan[0];
        auto second_event = desired_phase_plan->desired_phase_plan[1];

        auto current_time = streets_service::streets_clock_singleton::time_in_ms();
        // Assuming the first event start doesn't need to be planned for, we execute omit and hold for the next event. Resetting Hold ends the first event
        int64_t omit_execution_time = current_time + (first_event.end_time - current_time)/2;
        tsc_command_queue.push(create_omit_command(second_event.signal_groups, omit_execution_time));

        int64_t hold_execution_time = first_event.end_time;
        tsc_command_queue.push(create_hold_command(second_event.signal_groups, hold_execution_time));


        int event_itr = 1;

        while(event_itr < desired_phase_plan->desired_phase_plan.size() - 1)
        {
            auto current_event  = desired_phase_plan->desired_phase_plan[event_itr];
            auto next_event = desired_phase_plan->desired_phase_plan[event_itr + 1];

            omit_execution_time = current_event.start_time + (current_event.end_time - current_event.start_time)/2;
            tsc_command_queue.push(create_omit_command(next_event.signal_groups, omit_execution_time));

            hold_execution_time = current_event.end_time;
            tsc_command_queue.push(create_hold_command(next_event.signal_groups, hold_execution_time));

            event_itr++;
        }

        // Reset Hold and Omit for last event
        auto last_event = desired_phase_plan->desired_phase_plan.back();
        omit_execution_time = last_event.start_time + (last_event.end_time - last_event.start_time)/2;
        std::vector<int> empty_group = {};
        tsc_command_queue.push(create_omit_command(empty_group, omit_execution_time, true));

        hold_execution_time = last_event.end_time;
        tsc_command_queue.push(create_hold_command(empty_group, hold_execution_time, true));

        SPDLOG_DEBUG("Updated queue");

    }

    void control_tsc_state::update_tsc_control_queue(std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule_ptr,
        std::queue<snmp_cmd_struct>& tsc_command_queue) const
    {
        if(!phase_control_schedule_ptr)
        {
            SPDLOG_DEBUG("Phase control schedule is not initialized.");
            return;
        }
        if(phase_control_schedule_ptr->is_clear_current_schedule)
        {
            SPDLOG_DEBUG("Clear SNMP command queue!");
            //ToDo: Send clear all scheduled jobs or clear all commands on the update command queue
        }else{
            std::stringstream ss;
            ss << *phase_control_schedule_ptr;    
            SPDLOG_DEBUG("Update SNMP command queue with new phase control schedule commands: {0}", ss.str());                    
            //ToDo: Update command queue with the new phase control schedule commands
        }
    }

    snmp_cmd_struct control_tsc_state::create_omit_command(const std::vector<int>& signal_groups, int64_t start_time, bool is_reset) const
    {
        if(!is_reset)
        {
            uint8_t omit_val = 255; //Initialize to 11111111

            for(auto signal_group : signal_groups)
            {
                int phase = _tsc_state->get_phase_number(signal_group);
                // Omit all phases except the ones in the given movement group
                // For Omit only given phase bits are 0. Subtract 1 since phases range from 1-8.
                omit_val &= ~(1 << (phase - 1));
                
            }

            snmp_cmd_struct command(snmp_client_worker_, start_time, snmp_cmd_struct::control_type::Omit, static_cast<int64_t>(omit_val));
            return command;
        }
        else
        {
            snmp_cmd_struct command(snmp_client_worker_, start_time, snmp_cmd_struct::control_type::Omit, static_cast<int64_t>(0));
            return command;
        }

    }

    snmp_cmd_struct control_tsc_state::create_hold_command(const std::vector<int>& signal_groups, int64_t start_time, bool is_reset) const
    {
        if(!is_reset)
        {
            uint8_t hold_val = 0;   //Initialize to 00000000

            for(auto signal_group : signal_groups)
            {
                int phase = _tsc_state->get_phase_number(signal_group);
                // Hold phases in the given movement group
                //For Hold only given phase bits are 1. Subtract 1 since phases range from 1-8.
                hold_val |= (1 << ( phase - 1));
                
            }

            snmp_cmd_struct command(snmp_client_worker_, start_time, snmp_cmd_struct::control_type::Hold, static_cast<int64_t>(hold_val));
            return command;
        }
        else
        {
            snmp_cmd_struct command(snmp_client_worker_, start_time, snmp_cmd_struct::control_type::Hold, static_cast<int64_t>(0));
            return command;
        }
    }

    std::string snmp_cmd_struct::get_cmd_info() const{

        std::string command_type;
        if(control_type_ == control_type::Hold){
            command_type = "Hold";
        }
        else{
            command_type = "Omit";
        }
        std::string execution_start_time = std::to_string(start_time_);

        // Convert value set to phases nums
        std::string value_set = std::to_string(set_val_.val_int);

        return "control_cmd_type:" + command_type + ";execution_start_time:" + execution_start_time + ";value_set:" + value_set;
        
    }


}