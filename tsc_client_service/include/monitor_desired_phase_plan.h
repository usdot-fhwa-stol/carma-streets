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
        /**
         * @brief Process first desired green signal group and duration in desired phase plan. Populate 
         * movement_event list of current movement_state with movement_events for first entry in desired 
         * phase plan.
         * 
         * @param cur_movement_state_ref reference to movement_state inside SPaT for which to populate
         * movement_event list.
         * @param desired_sg_green_timing first entry in desired phase plan which contains information 
         * about a green duration and the signal groups to which it applies.
         * @param tsc_state_ptr tsc_state information about traffic signal controller configuration including
         * red clearance and yellow change duraction.
         */
        void process_first_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                        const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, 
                                        const std::shared_ptr<tsc_state> tsc_state_ptr) const;
        /**
         * @brief Process all desired green signal group and duration in desired phase plan that are not the first entry.
         * Populate movement_event list of current movement_state with movement_events for all entries after first entry
         * in desired phase plan.
         * 
         * @param cur_movement_state_ref reference to movement_state inside SPaT for which to populate 
         * movement_event list.
         * @param desired_sg_green_timing entry in desired phase plan which contains information 
         * about a green duration and the signal groups to which it applies.
         * @param tsc_state_ptr tsc_state information about traffic signal controller configuration including
         * red clearance and yellow change duraction.
         */
        void process_second_onward_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                                const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, 
                                                const std::shared_ptr<tsc_state> tsc_state_ptr) const;
        /**
         * @brief Populate movement_event with movement_phase_state, start_time and end_time(min_end_time).
         * 
         * @param movement_event_to_populate reference to movement_event to populate.
         * @param phase_state enumeration to describe phase state.
         * @param start_time_epoch start time of movement event in milliseconds epoch time.
         * @param end_time_epoch end time of movement event in milliseconds epoch time.
         */
        void populate_movement_event(
                                    signal_phase_and_timing::movement_event &movement_event_to_populate, 
                                    const signal_phase_and_timing::movement_phase_state &phase_state, 
                                    const uint64_t start_time_epoch, 
                                    const uint64_t end_time_epoch) const;
        /**
         * @brief Populate movement_event list for given movement_state with green event, yellow change event, and red clearance 
         * event.
         * 
         * @param cur_movement_state_ref reference to movement_state.
         * @param desired_sg_green_timing desired phase plan entry for which to populate green, yellow and red clearance events.
         * @param desired_yellow_duration yellow duration configured for signal group of movement_state.
         * @param desired_red_clearance red clearance configured for signal group of movement_state.
         */
        void append_full_green_yellow_red_phases_by_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                                                const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, 
                                                                const int desired_yellow_duration,
                                                                const int desired_red_clearance) const;
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
