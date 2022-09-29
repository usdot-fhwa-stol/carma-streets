#include "monitor_desired_phase_plan.h"

namespace traffic_signal_controller_service
{
    void monitor_desired_phase_plan::update_desired_phase_plan(const std::string &payload)
    {
        desired_phase_plan_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan_ptr->fromJson(payload);
    }

    std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> monitor_desired_phase_plan::get_desired_phase_plan_ptr() const
    {
        return desired_phase_plan_ptr;
    }

    void monitor_desired_phase_plan::update_spat_future_movement_events(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const std::shared_ptr<tsc_state> tsc_state_ptr) const
    {
        if (tsc_state_ptr == nullptr || spat_ptr == nullptr)
        {
            throw monitor_desired_phase_plan_exception("SPAT and TSC state pointers cannot be null. SKIP prcessing!");
        }
        if (spat_ptr->intersections.empty())
        {
            throw monitor_desired_phase_plan_exception("Intersections cannot be empty!");
        }
        auto states = spat_ptr->intersections.front().states;

        if (states.empty())
        {
            throw monitor_desired_phase_plan_exception("Intersections states cannot be empty!");
        }

        if (desired_phase_plan_ptr == nullptr || desired_phase_plan_ptr->desired_phase_plan.empty())
        {
            throw monitor_desired_phase_plan_exception("Desired phase plan is empty. No update.");
        }

        auto sg_yellow_duration_red_clearnace_map_ptr = std::make_shared<std::unordered_map<int,streets_tsc_configuration::signal_group_configuration>>();
        for (const auto sg_state : tsc_state_ptr->get_signal_group_state_map())
        {
            int sg_id = sg_state.first;
            int sg_yellow_duration = sg_state.second.yellow_duration;
            int sg_red_clearance = sg_state.second.red_clearance;
            streets_tsc_configuration::signal_group_configuration sg_state_item;
            sg_state_item.yellow_change_duration = sg_yellow_duration;
            sg_state_item.red_clearance = sg_red_clearance;
            sg_yellow_duration_red_clearnace_map_ptr->insert({sg_id, sg_state_item});
        }

        // Loop through desired phase plan
        bool is_procssing_first_desired_green = true;
        for (const auto &desired_sg_green_timing : desired_phase_plan_ptr->desired_phase_plan)
        {
            if (desired_sg_green_timing.signal_groups.empty())
            {
                throw monitor_desired_phase_plan_exception("Desired phase plan signal group ids list is empty. No update.");
            }

            // Loop through current spat and assuming that current spat only has the current movement event and does not contain future movement events
            for (const auto &movement_state : states)
            {
                int current_signal_group_id = movement_state.signal_group;

                // Before we add future movement events to the movement event list, the current movement event list should only contains the current movement event
                if (movement_state.state_time_speed.size() > 1)
                {
                    throw monitor_desired_phase_plan_exception("Movement event list has more than one events, not usable when adding future movement events. Associated with Signal Group: " + std::to_string(current_signal_group_id));
                }

                // Get movement_state by reference. With this reference, it can update the original SPAT movement state list
                auto &cur_movement_state_ref = spat_ptr->intersections.front().get_movement(current_signal_group_id);
                // Processing the next current or first desired future movement event from the desired phase plan
                if (is_procssing_first_desired_green)
                {
                    spat_ptr->process_first_desired_green(cur_movement_state_ref, desired_sg_green_timing, sg_yellow_duration_red_clearnace_map_ptr);
                }
                else
                {
                    // Processing the next future desired future movement event from the desired phase plan
                    spat_ptr->process_second_onward_desired_green(cur_movement_state_ref, desired_sg_green_timing, sg_yellow_duration_red_clearnace_map_ptr);
                }
            }
            is_procssing_first_desired_green = false;
        }
    }
}