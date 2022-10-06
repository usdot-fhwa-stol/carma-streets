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

        auto sg_yellow_duration_red_clearnace_map_ptr = std::make_shared<std::unordered_map<int, streets_tsc_configuration::signal_group_configuration>>();
        for (const auto& [cur_sg_id, cur_sg_state] : tsc_state_ptr->get_signal_group_state_map())
        {
            streets_tsc_configuration::signal_group_configuration sg_state_item;
            sg_state_item.yellow_change_duration = cur_sg_state.yellow_duration;
            sg_state_item.red_clearance = cur_sg_state.red_clearance;
            sg_yellow_duration_red_clearnace_map_ptr->insert({cur_sg_id, sg_state_item});
        }

        spat_ptr->update_spat_with_candidate_dpp(*desired_phase_plan_ptr, sg_yellow_duration_red_clearnace_map_ptr);
    }
}