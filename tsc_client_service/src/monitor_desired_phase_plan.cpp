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
        auto intersection = spat_ptr->get_intersection();
        auto states = intersection.states;

        if (states.empty())
        {
            throw monitor_desired_phase_plan_exception("Intersections states cannot be empty!");
        }
        
        if (desired_phase_plan_ptr == nullptr || desired_phase_plan_ptr->desired_phase_plan.empty())
        {
            throw monitor_desired_phase_plan_exception("Desired phase plan is empty. No update.");
        }

        spat_ptr->update_spat_with_candidate_dpp(*desired_phase_plan_ptr, tsc_state_ptr);
    }
}