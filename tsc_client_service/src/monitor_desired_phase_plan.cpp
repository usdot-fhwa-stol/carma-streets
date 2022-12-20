#include "monitor_desired_phase_plan.h"

namespace traffic_signal_controller_service
{
    monitor_desired_phase_plan::monitor_desired_phase_plan(const std::shared_ptr<snmp_client> client) :  _snmp_client(client) {

    }
    void monitor_desired_phase_plan::update_desired_phase_plan(const std::string &payload)
    {
        desired_phase_plan_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan_ptr->fromJson(payload);
    }

    std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> monitor_desired_phase_plan::get_desired_phase_plan_ptr() const
    {
        return desired_phase_plan_ptr;
    }

    void monitor_desired_phase_plan::update_spat_future_movement_events(const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const std::shared_ptr<tsc_state> tsc_state_ptr) 
    {
        if (tsc_state_ptr == nullptr || 
            spat_ptr == nullptr || 
            tsc_state_ptr->get_tsc_config_state() == nullptr ||
            tsc_state_ptr->get_tsc_config_state()->tsc_config_list.empty())
        {
            throw monitor_desired_phase_plan_exception("SPAT and TSC state pointers cannot be null. SKIP processing!");
        }

        if (spat_ptr->get_intersection().states.empty())
        {
            throw monitor_desired_phase_plan_exception("Intersections states cannot be empty!");
        }
        if (desired_phase_plan_ptr == nullptr || desired_phase_plan_ptr->desired_phase_plan.empty())
        {
            // If no current green -> then either yellow change or red clearance active
            // => Fix next green phase in SPaT
            std::vector<signal_phase_and_timing::movement_state> green_phases_present;
            auto state = spat_ptr->get_intersection();
            for (const auto &movement : state.states) {
                if ( movement.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed) {
                    
                    green_phases_present.push_back(movement);
                }
            }

            if ( green_phases_present.empty() ) {
                if (last_green_served.empty() ) {
                    throw monitor_desired_phase_plan_exception("No information on previous green!");
                }
                fix_upcoming_green(spat_ptr, tsc_state_ptr);
            } else {
                // Track which greens were served last and update this value.
                if ( last_green_served.empty() || green_phases_present.front().signal_group != last_green_served.front().signal_group || 
                    green_phases_present.back().signal_group != last_green_served.front().signal_group) {
                    last_green_served = green_phases_present;
                    SPDLOG_DEBUG("Last served greens set to {0} and {0}!", 
                        last_green_served.front().signal_group,
                        last_green_served.size() == 1 ? 0 : last_green_served.back().signal_group );
                }
                fix_upcoming_yell_red(spat_ptr, tsc_state_ptr, green_phases_present);
            }
        } else {
            
            // Loop through the desired phase plan and remove an event if the end time is past current
            bool no_expired_events = false;
            while( !no_expired_events && desired_phase_plan_ptr != nullptr && !desired_phase_plan_ptr->desired_phase_plan.empty()){
                // Check first event in desired phase plan and remove if expired
                uint64_t cur_time_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                if(cur_time_since_epoch < desired_phase_plan_ptr->desired_phase_plan.front().end_time){
                    no_expired_events = true;
                }
                else{
                    // Warning for removing entries from dpp
                    SPDLOG_WARN("Current time is greater than event end time {0}, removing expired entry from desired phase plan", desired_phase_plan_ptr->desired_phase_plan.front().end_time);
                    desired_phase_plan_ptr->desired_phase_plan.erase(desired_phase_plan_ptr->desired_phase_plan.begin());
                }
            }
            SPDLOG_INFO("Updating SPaT with SO requested DPP : \n{0}", desired_phase_plan_ptr->toJson());

            spat_ptr->update_spat_with_candidate_dpp(*desired_phase_plan_ptr, tsc_state_ptr->get_tsc_config_state());
        }
    }

    void monitor_desired_phase_plan::fix_upcoming_green(  const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, 
            const std::shared_ptr<tsc_state> tsc_state) {
        // Find start time of next green
        bool is_yellow_change = false;
        // Start time for fixed up coming green
        uint64_t start_time_epoch_ms = 0;
        // Current time used for any calculations
        uint64_t cur_time_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        auto state = spat_ptr->get_intersection();

        // Check if current spat includes yellow clearance
        for (const auto &movement : state.states) {
            // Found yellow states
            if ( movement.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::protected_clearance ) {
                is_yellow_change = true;
                auto yellow_phase_configuration =tsc_state->get_signal_group_state_map().find(movement.signal_group)->second;
                // Start time of upcoming green will be start of yellow + yellow duration + red clearance 
                uint64_t local_start_time_epoch = movement.state_time_speed.front().timing.get_epoch_start_time() + yellow_phase_configuration.yellow_duration + yellow_phase_configuration.red_clearance;
                if ( local_start_time_epoch > start_time_epoch_ms) {
                    // If there are multiple movement groups in yellow, start time will be the largest end of yellow + red clearance time
                    start_time_epoch_ms = local_start_time_epoch;
                }
                // If service starts on yellow change, set last green served to yellow change.
                if ( last_green_served.empty() ) {
                    last_green_served.push_back( movement);
                    SPDLOG_DEBUG("Last served greens set to {0} and {0}!", 
                        last_green_served.front().signal_group,
                        last_green_served.size() == 1 ? 0 : last_green_served.back().signal_group );
                }
            }
        }
        // If it does not include a green and does not include a yellow assume it is in all red clearance 
        if ( !is_yellow_change ) {
            for (const auto &movement : state.states) {
                // Found red states
                if ( (movement.signal_group == last_green_served.front().signal_group 
                    || movement.signal_group == last_green_served.back().signal_group) 
                    && movement.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::stop_and_remain ) {
                    auto red_phase_configuration =tsc_state->get_signal_group_state_map().find(movement.signal_group)->second;
                    // Start time is red clearance start + red clearance if start time is > current time
                    uint64_t local_start_time_epoch = movement.state_time_speed.front().timing.get_epoch_start_time() + red_phase_configuration.red_clearance;
                    // If red phase start + red clearance is greater than or equal to current time this phase is in red clearance
                    // and will get green next.
                    start_time_epoch_ms = local_start_time_epoch;

                    if ( local_start_time_epoch < cur_time_since_epoch ) {
                        SPDLOG_ERROR("Error state SPaT : {0}", spat_ptr->toJson());
                        SPDLOG_ERROR("Error state last green served : {0}, {1}", last_green_served.front().signal_group, last_green_served.back().signal_group);
                        throw monitor_desired_phase_plan_exception("Local start time is less than current time which means this is not an upcoming green!");
                    }

                }
            }
        }
        // Once start time is determine. We must determine next phase.
        snmp_response_obj response;
        response.type = snmp_response_obj::response_type::INTEGER;
        _snmp_client->process_snmp_request(ntcip_oids::PHASE_STATUS_GROUP_PHASE_NEXT,traffic_signal_controller_service::request_type::GET, response );
        
        // Generate Desired Phase Plan with next green fixed.
        streets_desired_phase_plan::streets_desired_phase_plan one_fixed_green;
        one_fixed_green.timestamp = cur_time_since_epoch;
        streets_desired_phase_plan::signal_group2green_phase_timing fixed_green;
        fixed_green.start_time = start_time_epoch_ms;
        
        /**
         * Response value is 8 bit int in which each bit is interpreted individually as 1 or 0. 1 
         * indicates that the vehicle phase for that bit is committed to be next. 0 indicates this 
         * phase is not committed to be next.
         * 
         * bit 0 represent vehicle phase 1
         * bit 1 represent vehicle phase 2
         * bit 2 represent vehicle phase 3
         * bit 3 represent vehicle phase 4
         * bit 4 represent vehicle phase 5
         * bit 5 represent vehicle phase 6
         * bit 6 represent vehicle phase 7
         * bit 7 represent vehicle phase 8
         * 
         */
        for (uint i = 0; i < 8; ++i) {
            if (((response.val_int >> i) & 1) == 1) {
                // Add any signal group for phase that has bit as 1
                auto signal_group_id = tsc_state->get_vehicle_signal_group_id(i+1);
                
                fixed_green.signal_groups.push_back(signal_group_id);
            }
        }
        if ( fixed_green.signal_groups.size() > 2 ) {
            throw monitor_desired_phase_plan_exception( "More than two phases were found to be next!");
        }
        else if ( fixed_green.signal_groups.size() < 1) {
            throw monitor_desired_phase_plan_exception("No signal groups were found to be next");
        }
        // Calculating the end of the fixed green by using min green configuration
        // for all signal groups included in the movement group that is assigned the 
        // next green.
        uint64_t fixed_green_end_time_epoch_ms = 0;
        for ( const auto &s_g_id : fixed_green.signal_groups ) {
            auto sig_group_config = tsc_state->get_signal_group_state_map().find(s_g_id)->second;
            uint64_t local_end_time_epoch_ms = fixed_green.start_time + sig_group_config.min_green;
            if ( local_end_time_epoch_ms > fixed_green_end_time_epoch_ms ) {
                // find max of green start + min green to assign end time
                fixed_green_end_time_epoch_ms = local_end_time_epoch_ms;
            }
        }
        fixed_green.end_time = fixed_green_end_time_epoch_ms;
        one_fixed_green.desired_phase_plan.push_back(fixed_green);
        SPDLOG_DEBUG("Updating SPaT with fixed green : \n{0}", one_fixed_green.toJson());
        spat_ptr->update_spat_with_candidate_dpp(one_fixed_green, tsc_state->get_tsc_config_state());

    }


    void monitor_desired_phase_plan::fix_upcoming_yell_red(  const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, 
            const std::shared_ptr<tsc_state> tsc_state, const std::vector<signal_phase_and_timing::movement_state> &green_phases)  const{
        streets_desired_phase_plan::signal_group2green_phase_timing fixed_green;

        if ( green_phases.size() > 2 || green_phases.size() < 1) {
            throw monitor_desired_phase_plan_exception( "More than two or less than one present phases were found to be green!");
        } else {
            for ( const auto &phase : green_phases ) {
                fixed_green.signal_groups.push_back(phase.signal_group);
            }
        }
        // Current time used for any calculations
        streets_desired_phase_plan::streets_desired_phase_plan one_fixed_green;
        uint64_t cur_time_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        auto state = spat_ptr->get_intersection();
        // Get green phase configuration to get min green.
        auto green_phase_config =tsc_state->get_signal_group_state_map().find(green_phases.front().signal_group)->second;

            
        // Generate Desired Phase Plan with next green fixed.
        one_fixed_green.timestamp = cur_time_since_epoch;
        fixed_green.start_time = green_phases.front().state_time_speed.front().timing.get_epoch_start_time();
        fixed_green.end_time =  fixed_green.start_time + green_phase_config.min_green;
        one_fixed_green.desired_phase_plan.push_back(fixed_green);
        SPDLOG_INFO("Updating SPaT with fixed green : \n{0}", one_fixed_green.toJson());
        spat_ptr->update_spat_with_candidate_dpp(one_fixed_green, tsc_state->get_tsc_config_state());    
    }
}