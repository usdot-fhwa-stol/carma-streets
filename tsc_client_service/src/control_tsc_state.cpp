#include "control_tsc_state.h"
#include "control_tsc_state_exception.h"
#include <chrono>

namespace traffic_signal_controller_service
{
    control_tsc_state::control_tsc_state(std::shared_ptr<snmp_client> snmp_client, std::unordered_map<int, int>& signal_group_to_phase_map, 
    std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan>& desired_phase_plan)  
                            : snmp_client_worker_(snmp_client), signal_group_2ped_phase_map_(signal_group_to_phase_map)
    {
        // Restart control if first event timestamp has changed, since the desired phase plan has changed
        if(!desired_phase_plan_ || desired_phase_plan->desired_phase_plan.front() != desired_phase_plan_->desired_phase_plan.front())
        {
            desired_phase_plan_ = desired_phase_plan;
            first_event_start_time_ = desired_phase_plan_->desired_phase_plan.front().start_time;
            run();
        }
    }

    void control_tsc_state::run()
    {
        if(desired_phase_plan_->desired_phase_plan.empty()){
            SPDLOG_DEBUG("No events in desired phase plan");
            return;
        }
        // Omit and Hold for first movement group in plan
        auto first_event = desired_phase_plan_->desired_phase_plan[0];
        if(!omit_and_hold_signal_groups(first_event.signal_groups)){
            throw control_tsc_state_exception("Could not set state for movement group 0 in desired phase plan");
        }

        int event_itr = 0;
        // At the end time of the current event, prepare for next event. So control ends at second to last event
        while(event_itr < desired_phase_plan_->desired_phase_plan.size() - 2)
        {
            auto event  = desired_phase_plan_->desired_phase_plan[event_itr];
            auto next_event = desired_phase_plan_->desired_phase_plan[event_itr + 1];

            // Check no repeated signal groups in adjacent events
            for(int i = 0 ; i < event.signal_groups.size(); ++i)
            {
                auto it = std::find(next_event.signal_groups.begin(), next_event.signal_groups.end(), event.signal_groups[i]);
                if(it != next_event.signal_groups.end())
                {
                    SPDLOG_ERROR("TSC Service assumes adjacent events dont have the same signal_group. Given Desired phase plan fails this assumption");
                    throw control_tsc_state_exception("Repeating signal group found in adjacent events. Desired phase plan cannot be set");
                }
            }

            std::chrono::duration<long> start_time(event.start_time);
            std::chrono::time_point<std::chrono::system_clock> event_start_time(start_time);

            std::chrono::duration<long> end_time(event.end_time);
            std::chrono::time_point<std::chrono::system_clock> event_end_time(end_time);

            if(start_time < std::chrono::system_clock::now().time_since_epoch() || end_time < std::chrono::system_clock::now().time_since_epoch())
            {
                SPDLOG_ERROR("TSC Service assumes desired phase plan does not have overlapping events. Given Desired plan fails assumption");
                throw control_tsc_state_exception("Overlapping timings. Desired phase plan cannot be set");
            }

            // sleep thread till end time
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - std::chrono::system_clock::now().time_since_epoch());
            std::this_thread::sleep_for(duration);
            
            omit_and_hold_signal_groups(next_event.signal_groups);

            event_itr++;
            
        }
    }

    bool control_tsc_state::omit_and_hold_signal_groups(std::vector<int> signal_groups)
    {
        
        request_type request_type = request_type::SET;
        // Omit all phases except the ones in the given movement group
        int phase_one = signal_group_2ped_phase_map_[signal_groups[0]];
        int phase_two = signal_group_2ped_phase_map_[signal_groups[1]];

        uint8_t omit_val = 255; //Initialize to 11111111
        uint8_t hold_val = 0;   //Initialize to 00000000

        for(auto signal_group : signal_groups)
        {
            int phase = signal_group_2ped_phase_map_[signal_group];
            // For Omit only given phase bits are 0. Subtract 1 since phases range from 1-8.
            omit_val &= ~(1 << (phase - 1));
            //For Hold only given phase bits are 1. Subtract 1 since phases range from 1-8.
            hold_val |= (1 << ( phase - 1));
            
        }
        
        snmp_response_obj omit_control_type;
        omit_control_type.type = snmp_response_obj::response_type::INTEGER;
        omit_control_type.val_int = static_cast<int64_t>(omit_val);

        snmp_response_obj hold_control_type;
        hold_control_type.type = snmp_response_obj::response_type::INTEGER;
        hold_control_type.val_int = static_cast<int64_t>(hold_val);

        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_OMIT_CONTROL, request_type, omit_control_type))
        {
            return false;
        }

        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::PHASE_HOLD_CONTROL, request_type, hold_control_type))
        {
            return false;
        }
        
        return true;
        
    }
}