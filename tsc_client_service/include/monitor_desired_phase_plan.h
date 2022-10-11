#pragma once
#include "streets_desired_phase_plan.h"
#include "spat.h"
#include "monitor_tsc_state.h"
#include "monitor_desired_phase_plan_exception.h"
#include "spdlog/spdlog.h"

namespace traffic_signal_controller_service
{
    class monitor_desired_phase_plan
    {
    private:
        std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan_ptr;

    public:
        /**
         * @brief Construct a new monitor desired phase plan object
         * 
         */
        monitor_desired_phase_plan() = default;
        /**
         * @brief Destroy the monitor desired phase plan object
         * 
         */
        ~monitor_desired_phase_plan() = default;
        /**
         * @brief Update desired_phase_plan object with json received string message.
         * 
         * @param payload 
         */
        void update_desired_phase_plan(const std::string& payload);
        /**
         * @brief Update spat movement_event list with desired phase plan information.
         * 
         * @param spat_ptr shared pointer to spat object.
         * @param tsc_state_ptr shared pointer to tsc_state object.
         */
        void update_spat_future_movement_events(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, 
                                                const std::shared_ptr<tsc_state> tsc_state_ptr) const;
        /**
         * @brief Returns shared pointer to desired_phase_plan object.
         * 
         * @return std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> 
         */
        std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> get_desired_phase_plan_ptr() const;
    };

}
