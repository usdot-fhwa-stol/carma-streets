#include "control_tsc_state.h"
#include <chrono>

namespace traffic_signal_controller_service
{
    control_tsc_state::control_tsc_state(std::shared_ptr<snmp_client> snmp_client, const std::unordered_map<int, int>& signal_group_to_phase_map)  
                            : snmp_client_worker_(snmp_client), signal_group_2ped_phase_map_(signal_group_to_phase_map)
    {
                
    }

    void control_tsc_state::update_tsc_control_queue(std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan,
                                             std::shared_ptr<std::queue<tsc_control_struct>> tsc_command_queue_ptr)
    {
        if(desired_phase_plan->desired_phase_plan.empty()){
            SPDLOG_DEBUG("No events in desired phase plan");
            return;
        }
        // Omit and Hold for first movement group in plan
        auto first_event = desired_phase_plan->desired_phase_plan[0];
        auto first_event_execution_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        tsc_command_queue_ptr->push(omit_and_hold_signal_groups(first_event.signal_groups, first_event_execution_time));

        int event_itr = 0;
        // At the end time of the current event, prepare for next event. So control ends at second to last event
        while(event_itr < desired_phase_plan->desired_phase_plan.size() - 1)
        {
            auto event  = desired_phase_plan->desired_phase_plan[event_itr];
            auto next_event = desired_phase_plan->desired_phase_plan[event_itr + 1];

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

            auto current_time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            auto current_event_start_time  = std::chrono::milliseconds(event.start_time);
            auto current_event_end_time = std::chrono::milliseconds(event.end_time);

            if(current_event_start_time < current_time_in_ms || current_event_end_time < current_time_in_ms)
            {
                SPDLOG_ERROR("TSC Service assumes desired phase plan does not have overlapping events. Given Desired plan fails assumption");
                throw control_tsc_state_exception("Overlapping timings. Desired phase plan cannot be set");
            }

            // sleep thread till end time
            // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_event_end_time - std::chrono::system_clock::now().time_since_epoch());
            // std::this_thread::sleep_for(duration);
            
            // Add object to queue
            tsc_command_queue_ptr->push(omit_and_hold_signal_groups(next_event.signal_groups, current_event_end_time.count()));

            event_itr++;
            
        }
    }

    tsc_control_struct control_tsc_state::omit_and_hold_signal_groups(std::vector<int> signal_groups, int64_t start_time)
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

        tsc_control_struct command(snmp_client_worker_, static_cast<int64_t>(omit_val), static_cast<int64_t>(hold_val), start_time);

        return command;
        
    }
}