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

    void monitor_desired_phase_plan::update_spat_future_movement_events(const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const std::shared_ptr<tsc_state> tsc_state_ptr) const
    {
        if (tsc_state_ptr == nullptr || spat_ptr == nullptr || tsc_state_ptr->get_tsc_config_state() == nullptr)
        {
            throw monitor_desired_phase_plan_exception("SPAT and TSC state pointers cannot be null. SKIP prcessing!");
        }

        if (spat_ptr->get_intersection().states.empty())
        {
            throw monitor_desired_phase_plan_exception("Intersections states cannot be empty!");
        }
        if (desired_phase_plan_ptr == nullptr || desired_phase_plan_ptr->desired_phase_plan.empty())
        {
            // If no current green -> then either yellow change or red clearance active
            // => Fix next green phase in SPaT
            bool is_green_present = false;
            auto state = spat_ptr->get_intersection();
            for (const auto &movement : state.states) {
                if ( movement.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed) {
                    is_green_present = true;
                    break;
                }
            }
            if (!is_green_present ) {
                fix_upcoming_green(spat_ptr, tsc_state_ptr);
            }
        } else {
            spat_ptr->update_spat_with_candidate_dpp(*desired_phase_plan_ptr, tsc_state_ptr->get_tsc_config_state());
        }
    }

    void monitor_desired_phase_plan::fix_upcoming_green(  const std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, 
            const std::shared_ptr<tsc_state> tsc_state)  const{
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
                // Start time of upcoming green will be end of yellow + red clearance 
                uint64_t local_start_time_epoch = movement.state_time_speed.front().timing.get_epoch_min_end_time() + yellow_phase_configuration.red_clearance;
                if ( local_start_time_epoch > start_time_epoch_ms) {
                    // If there are multiple movement groups in yellow, start time will be the largest end of yellow + red clearance time
                    start_time_epoch_ms = local_start_time_epoch;
                }
            }
        }
        // If it does not include a green and does not include a yellow assume it is in all red clearance 
        if ( !is_yellow_change ) {
            for (const auto &movement : state.states) {
                // Found red states
                if ( movement.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::stop_and_remain ) {
                    auto red_phase_configuration =tsc_state->get_signal_group_state_map().find(movement.signal_group)->second;
                    uint64_t local_start_time_epoch = movement.state_time_speed.front().timing.get_epoch_min_end_time();
                    // If red phase end - current time is less than or equal to red clearance then this phase is currently in red clearance
                    // and will get green next.
                    if ( local_start_time_epoch - cur_time_since_epoch <= (uint) (red_phase_configuration.red_clearance) ) {
                        start_time_epoch_ms = local_start_time_epoch;
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
        SPDLOG_DEBUG("Updating SPaT with DPP : \n{0}", one_fixed_green.toJson());
        spat_ptr->update_spat_with_candidate_dpp(one_fixed_green, tsc_state->get_tsc_config_state());

    }
}