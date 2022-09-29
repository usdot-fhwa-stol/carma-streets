#include "control_tsc_state.h"
#include <chrono>

namespace traffic_signal_controller_service
{
    control_tsc_state::control_tsc_state(std::shared_ptr<snmp_client> snmp_client, const std::unordered_map<int, int>& signal_group_to_phase_map)  
                            : snmp_client_worker_(snmp_client), signal_group_2ped_phase_map_(signal_group_to_phase_map)
    {
                
    }

    void control_tsc_state::update_tsc_control_queue(std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan,
                                             std::queue<tsc_control_struct>& tsc_command_queue)
    {
        if(desired_phase_plan->desired_phase_plan.empty()){
            SPDLOG_DEBUG("No events in desired phase plan");
            return;
        }
        
        //Reset queue
        tsc_command_queue = std::queue<tsc_control_struct>();
        // Add first event
        
        // Omit and Hold for first movement group in plan - skip if current phase is yellow 
        //if green - start time should be dpp_start_time - red_clearance - yellow duration
        auto first_event = desired_phase_plan->desired_phase_plan[0];
        auto first_event_end_time = std::chrono::milliseconds(first_event.start_time);
        auto first_event_execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        tsc_command_queue.push(omit_and_hold_signal_groups(first_event.signal_groups, first_event_execution_time, first_event_end_time.count(), true));
        

        int event_itr = 1;
        // At the end time of the current event, prepare for next event. So control ends at second to last event
        while(event_itr < desired_phase_plan->desired_phase_plan.size() - 1)
        {
            auto event  = desired_phase_plan->desired_phase_plan[event_itr];
            auto next_event = desired_phase_plan->desired_phase_plan[event_itr + 1];

            auto current_time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            auto current_event_start_time  = std::chrono::milliseconds(event.start_time);
            auto current_event_end_time = std::chrono::milliseconds(event.end_time);

            if(current_event_start_time < current_time_in_ms || current_event_end_time < current_time_in_ms)
            {
                SPDLOG_WARN("Event {0} in desired phase plan in expired, not adding to queue", event_itr);
                event_itr++;
                continue;
            }

            // Check no repeated signal groups in adjacent events
            for(auto signal_group : event.signal_groups)
            {
                auto it = std::find(next_event.signal_groups.begin(), next_event.signal_groups.end(), signal_group);
                if(it != next_event.signal_groups.end())
                {
                    SPDLOG_ERROR("TSC Service assumes adjacent events dont have the same signal_group. Given Desired phase plan fails this assumption");
                    throw control_tsc_state_exception("Repeating signal group found in adjacent events. Desired phase plan cannot be set");
                }
            }
            
            auto next_event_end_time = std::chrono::milliseconds(next_event.end_time);
            // Add object to queue
            tsc_command_queue.push(omit_and_hold_signal_groups(next_event.signal_groups, current_event_end_time.count(), next_event_end_time.count()));

            event_itr++;
            
        }
    }

    tsc_control_struct control_tsc_state::omit_and_hold_signal_groups(std::vector<int> signal_groups, int64_t start_time, int64_t end_time, bool execute_now)
    {
        uint8_t omit_val = 255; //Initialize to 11111111
        uint8_t hold_val = 0;   //Initialize to 00000000

        for(auto signal_group : signal_groups)
        {
            int phase = signal_group_2ped_phase_map_[signal_group];
            // Omit all phases except the ones in the given movement group
            // For Omit only given phase bits are 0. Subtract 1 since phases range from 1-8.
            omit_val &= ~(1 << (phase - 1));
            // Hold phases in the given movement group
            //For Hold only given phase bits are 1. Subtract 1 since phases range from 1-8.
            hold_val |= (1 << ( phase - 1));
            
        }

        tsc_control_struct command(snmp_client_worker_, static_cast<int64_t>(omit_val), static_cast<int64_t>(hold_val), start_time, end_time);
        command.execute_now_ = execute_now;

        return command;
        
    }

    bool control_tsc_state::reset_hold_and_omit ()
    {
        request_type type = request_type::SET;

        snmp_response_obj omit;
        omit.type = snmp_response_obj::response_type::INTEGER;
        omit.val_int = 0;

        snmp_response_obj hold;
        hold.type = snmp_response_obj::response_type::INTEGER;
        hold.val_int = 0;

        // Send Omit
        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_OMIT_CONTROL, type, omit)){return false;}
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // Send Hold
        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_HOLD_CONTROL, type, hold)){return false;}

        return true;
    }

}