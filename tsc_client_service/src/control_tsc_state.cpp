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
                                             std::queue<streets_snmp_cmd::snmp_cmd_struct>& tsc_command_queue) const
    {
        if(desired_phase_plan->desired_phase_plan.empty()){
            SPDLOG_DEBUG("No events in desired phase plan");
            return;
        }
        if(desired_phase_plan->desired_phase_plan.size() == 1){
            SPDLOG_DEBUG("TSC service assumes first event is already set, no update to queue required");
            return;
        }

        auto signal_groups_2phase_map = _tsc_state->get_signal_group_map();
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
        tsc_command_queue = std::queue<streets_snmp_cmd::snmp_cmd_struct>();

        // add Omit and Hold commands
        auto first_event = desired_phase_plan->desired_phase_plan[0];
        auto second_event = desired_phase_plan->desired_phase_plan[1];

        auto current_time = streets_service::streets_clock_singleton::time_in_ms();
        // Assuming the first event start doesn't need to be planned for, we execute omit and hold for the next event. Resetting Hold ends the first event
        int64_t omit_execution_time = current_time + (first_event.end_time - current_time)/2;
        //Omit all others signal groups other than the second_event event signal_groups
        std::vector<int> second_event_signal_groups_to_omit;
        for(auto& [sg, phase]: signal_groups_2phase_map)
        {
            //If the signal group is not in the second_event signal groups, add the signal group to the list of omit signal groups 
            if(std::find(second_event.signal_groups.begin(), second_event.signal_groups.end(), sg) == second_event.signal_groups.end() )
            {
                second_event_signal_groups_to_omit.push_back(sg);
            }
        }
        tsc_command_queue.push(create_snmp_command_by_signal_groups(second_event_signal_groups_to_omit, streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, omit_execution_time));

        int64_t hold_execution_time = first_event.end_time;
        tsc_command_queue.push(create_snmp_command_by_signal_groups(second_event.signal_groups,streets_snmp_cmd::PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, hold_execution_time));

        int event_itr = 1;
        while(event_itr < desired_phase_plan->desired_phase_plan.size() - 1)
        {
            auto current_event  = desired_phase_plan->desired_phase_plan[event_itr];
            auto next_event = desired_phase_plan->desired_phase_plan[event_itr + 1];

            omit_execution_time = current_event.start_time + (current_event.end_time - current_event.start_time)/2;
            //Omit all others signal groups other than the next_event event signal_groups
            std::vector<int> next_event_signal_groups_to_omit;
            for(auto& [sg, phase]: signal_groups_2phase_map)
            {
                //If the signal group is not in the next_event signal groups, add the signal group to the list of omit signal groups 
                if(std::find(next_event.signal_groups.begin(), next_event.signal_groups.end(), sg) == next_event.signal_groups.end() )
                {
                    next_event_signal_groups_to_omit.push_back(sg);
                }
            }
            tsc_command_queue.push(create_snmp_command_by_signal_groups(next_event_signal_groups_to_omit, streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, omit_execution_time));

            hold_execution_time = current_event.end_time;
            tsc_command_queue.push(create_snmp_command_by_signal_groups(next_event.signal_groups,streets_snmp_cmd::PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, hold_execution_time));

            event_itr++;
        }

        // Reset Hold and Omit for last event
        auto last_event = desired_phase_plan->desired_phase_plan.back();
        omit_execution_time = last_event.start_time + (last_event.end_time - last_event.start_time)/2;
        streets_snmp_cmd::streets_snmp_cmd_converter converter;
        tsc_command_queue.push(converter.create_snmp_reset_command(streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, omit_execution_time));

        hold_execution_time = last_event.end_time;
        tsc_command_queue.push(converter.create_snmp_reset_command(streets_snmp_cmd::PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, hold_execution_time));
        SPDLOG_DEBUG("Updated queue with front command {0}!", tsc_command_queue.front().get_cmd_info());

    }

    void control_tsc_state::update_tsc_control_queue(std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule_ptr,
        std::queue<streets_snmp_cmd::snmp_cmd_struct>& tsc_command_queue) const
    {
        if(!phase_control_schedule_ptr)
        {
            SPDLOG_ERROR("Phase control schedule is not initialized.");
            return;
        }
        if(phase_control_schedule_ptr->is_clear_current_schedule)
        {
            SPDLOG_INFO("Clear SNMP command queue!");
            // Clear all commands on the update command queue.
            std::queue<streets_snmp_cmd::snmp_cmd_struct> empty_queue;
            std::swap(tsc_command_queue, empty_queue);
        }else{
            std::stringstream ss;
            ss << *phase_control_schedule_ptr;    
            SPDLOG_DEBUG("Update SNMP command queue with new phase control schedule commands: {0}", ss.str());                    
            // Update command queue with the new phase control schedule commands.
            streets_snmp_cmd::streets_snmp_cmd_converter converter;
            auto snmp_cmd_queue = converter.create_snmp_cmds_by_phase_control_schedule(phase_control_schedule_ptr);
            std::swap(tsc_command_queue, snmp_cmd_queue);
        }
        SPDLOG_INFO("Updated Queue Size: {0}", tsc_command_queue.size());
    }

    streets_snmp_cmd::snmp_cmd_struct control_tsc_state::create_snmp_command_by_signal_groups(const std::vector<int>& signal_groups, streets_snmp_cmd::PHASE_CONTROL_TYPE phase_control_type, int64_t start_time) const
    {
        streets_snmp_cmd::streets_snmp_cmd_converter converter;
        std::vector<int> phases;
        for(auto signal_group : signal_groups)
        {
            int phase = _tsc_state->get_phase_number(signal_group);
            phases.push_back(phase);            
        }
        auto command = converter.create_snmp_command_by_phases(phases, phase_control_type, start_time);
        return command;
    }

    bool control_tsc_state::run_snmp_cmd_set_request(streets_snmp_cmd::snmp_cmd_struct& snmp_cmd) const
    {     
        if(!snmp_client_worker_)
        {
            throw control_tsc_state_exception("SNMP Client worker is not initialized.");
        }
        /*Type of request to be sent to the TSC, within this context it is always SET*/
        auto type = streets_snmp_cmd::REQUEST_TYPE::SET;

        if(snmp_cmd.control_type_ ==  streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
        {
            if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_OMIT_CONTROL, type, snmp_cmd.set_val_)){
                return false;
            }
        }
        else if (snmp_cmd.control_type_ ==  streets_snmp_cmd::PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
        {
            if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_HOLD_CONTROL, type, snmp_cmd.set_val_)){
                return false;
            }
        }
        else if (snmp_cmd.control_type_ ==  streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
        {
            if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_PEDESTRIAN_OMIT_CONTROL, type, snmp_cmd.set_val_)){
                return false;
            }
        }
        else if (snmp_cmd.control_type_ ==  streets_snmp_cmd::PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
        {
            if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_FORCEOFF_CONTROL, type, snmp_cmd.set_val_)){
                return false;
            }
        }
        else if (snmp_cmd.control_type_ ==  streets_snmp_cmd::PHASE_CONTROL_TYPE::CALL_PED_PHASES)
        {
            if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_PEDESTRIAN_CALL_CONTROL, type, snmp_cmd.set_val_)){
                return false;
            }
        }
        else if (snmp_cmd.control_type_ ==  streets_snmp_cmd::PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
        {
            if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_VEHICLE_CALL_CONTROL, type, snmp_cmd.set_val_)){
                return false;
            }
        }

        return true;
    }
}