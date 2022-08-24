#pragma once
#include "streets_desired_phase_plan.h"
#include "spat.h"
#include "monitor_tsc_state.h"
#include "spdlog/spdlog.h"

namespace traffic_signal_controller_service
{
    class monitor_desired_phase_plan
    {
    private:
        std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan_ptr;

        // Conversion constants
        int HOUR_TO_SECONDS_ = 3600;
        int SECOND_TO_MILLISECONDS_ = 1000;
        void process_first_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, const std::shared_ptr<tsc_state> tsc_state_ptr);
        void process_second_onward_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, const std::shared_ptr<tsc_state> tsc_state_ptr);
        void populate_movement_event(signal_phase_and_timing::movement_event &movemnet_event_to_populate, signal_phase_and_timing::movement_phase_state phase_state, uint64_t start_time_epoch, uint64_t end_time_epoch) const;
        void append_full_green_yellow_red_phases_by_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, uint16_t desired_yellow_duration, uint16_t desired_red_clearance) const;

    public:
        monitor_desired_phase_plan() = default;
        ~monitor_desired_phase_plan() = default;
        void update_desired_phase_plan(std::string payload);
        void update_spat_future_movement_events(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const std::shared_ptr<tsc_state> tsc_state_ptr);
        const std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan> get_desired_phase_plan_ptr() const;
    };

}
