#include "monitor_desired_phase_plan.h"

namespace traffic_signal_controller_service
{
    void monitor_desired_phase_plan::update_desired_phase_plan(std::string payload)
    {
        desired_phase_plan_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>();
        desired_phase_plan_ptr->fromJson(payload);
    }

    void monitor_desired_phase_plan::add_future_movement_events(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr, const std::shared_ptr<tsc_state> tsc_state_ptr)
    {
        if (tsc_state_ptr == nullptr || spat_ptr == nullptr)
        {
            SPDLOG_ERROR("SPAT and TSC state pointers cannot be null. SKIP prcessing!");
            return;
        }
        if (spat_ptr->intersections.empty())
        {
            SPDLOG_ERROR("Intersections cannot be empty");
            return;
        }
        auto states = spat_ptr->intersections.front().states;

        if (states.empty())
        {
            SPDLOG_ERROR("Intersections states cannot be empty");
            return;
        }

        if (desired_phase_plan_ptr->desired_phase_plan.empty())
        {
            SPDLOG_ERROR("Desired phase plan is empty. No update.");
            return;
        }

        // Loop through desired phase plan
        bool is_procssing_first_desired_green = true;
        for (const auto &desired_sg_green_timing : desired_phase_plan_ptr->desired_phase_plan)
        {
            if (desired_sg_green_timing.signal_groups.empty())
            {
                SPDLOG_ERROR("Desired phase plan signal group ids list is empty. No update.");
                return;
            }

            // Loop through current spat and assuming that current spat only has the current movement event and does not contain future movement events
            for (auto &movement_state : states)
            {
                int current_signal_group_id = movement_state.signal_group;

                // Before we add future movement events to the movement event list, the current movement event list should only contains the current movement event
                if (movement_state.state_time_speed.size() > 1)
                {
                    SPDLOG_ERROR("Movement event list has more than one events, not usable when adding future movement events. Associated with Signal Group: {0}", current_signal_group_id);
                    return;
                }

                // Get movement_state by reference. With this reference, it can update the original SPAT movement state list
                auto &cur_movement_state_ref = spat_ptr->intersections.front().get_movement(current_signal_group_id);

                // Processing the next current or first desired future movement event from the desired phase plan
                if (is_procssing_first_desired_green)
                {
                    process_first_desired_green(cur_movement_state_ref, desired_sg_green_timing, tsc_state_ptr);
                }
                else
                {
                    // Processing the next future desired future movement event from the desired phase plan
                    process_second_onward_desired_green(cur_movement_state_ref, desired_sg_green_timing, tsc_state_ptr);
                }
            }
            is_procssing_first_desired_green = false;
        }
    }

    void monitor_desired_phase_plan::process_first_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, const std::shared_ptr<tsc_state> tsc_state_ptr)
    {
        auto desired_green_signal_group_ids = desired_sg_green_timing.signal_groups;
        auto desired_green_start_time_epoch = desired_sg_green_timing.start_time;
        auto desired_green_end_time_epoch = desired_sg_green_timing.end_time;

        // Assuming signal groups(e.g [1,5]) within the same movement group(e.g. A) has the same red_clearance and yellow_duration
        uint16_t desired_yellow_duration = tsc_state_ptr->get_signal_group_state_map().at(desired_green_signal_group_ids.front()).yellow_duration;
        uint16_t desired_red_clearance = tsc_state_ptr->get_signal_group_state_map().at(desired_green_signal_group_ids.front()).red_clearance;

        // If SPAT Current signal group id is in the current desired phase plan signal group ids, process desired green event for the SPAT current movement event
        if (std::count(desired_green_signal_group_ids.begin(), desired_green_signal_group_ids.end(), cur_movement_state_ref.signal_group))
        {
            /**
             * @brief If the SPAT current movement event is GREEN event state,
             * - Updating the currnet GREEN event end time with desired green end time.
             * - Add YELLOW movement event with start time equals to desired green end time, and end time equals to start time plus desired yellow duration
             * - Add RED movement event with start time equals to the above YELLOW end time, and end time equals to start time plus desired red clearance
             */
            // GREEN
            if (cur_movement_state_ref.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed) // GREEN
            {
                // Updating GREEN movement event
                cur_movement_state_ref.state_time_speed.front().timing.set_min_end_time(desired_green_end_time_epoch);

                // Add YELLOW movement event
                signal_phase_and_timing::movement_event yellow_movement_event;
                uint64_t calculated_yellow_end_time_epoch = desired_green_end_time_epoch + desired_yellow_duration;
                populate_movement_event(yellow_movement_event, signal_phase_and_timing::movement_phase_state::protected_clearance, desired_green_end_time_epoch, calculated_yellow_end_time_epoch);
                cur_movement_state_ref.state_time_speed.push_back(yellow_movement_event);

                // Add RED movement event
                signal_phase_and_timing::movement_event red_movement_event;
                populate_movement_event(red_movement_event, signal_phase_and_timing::movement_phase_state::stop_and_remain, calculated_yellow_end_time_epoch, calculated_yellow_end_time_epoch + desired_red_clearance);
                cur_movement_state_ref.state_time_speed.push_back(red_movement_event);
            }
            /****
             * If SPAT current movement state is RED event state,
             * - Updating red event state end time to desired green start time.
             * - Adding GREEN event state start time to desired green start time and end time equals to desired green end time.
             * - Add YELLOW event state start time to desired green end time and end time euqals to start time + desired yellow duration.
             * - Add RED event state start time to the above YELLO end time and end time euqals to start time + desired red clearance.
             * ***/
            else if (cur_movement_state_ref.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::stop_and_remain) // RED
            {
                // Updating RED movement event
                cur_movement_state_ref.state_time_speed.front().timing.set_min_end_time(desired_green_start_time_epoch);

                // Append GREEN, YELLOW, and RED movement events
                append_full_green_yellow_red_phases_by_desired_green(cur_movement_state_ref, desired_sg_green_timing, desired_yellow_duration, desired_red_clearance);
            }
            else
            {
                SPDLOG_ERROR("SPAT current movement event has to be green or red event state!");
                return;
            }
        }
        else
        {
            // SPAT Current signal group id is NOT in the current desired phase plan signal group ids
            /**
             * @brief If the SPAT current movement state is RED,
             * - Updating the current RED movement event end time equals to desired green end time + yellow duration + red clearance
             */
            if (cur_movement_state_ref.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::stop_and_remain) // RED
            {
                // Updating the current RED event state
                cur_movement_state_ref.state_time_speed.front().timing.set_min_end_time(desired_green_end_time_epoch + desired_yellow_duration + desired_red_clearance);
            }
            /**
             * @brief
             * If the SPAT current movement state is YELLOW,
             * - Updating the YELLOW movement event end time equals to YELLOW start time + yellow duration of current spat movement state signal group id.
             * - Add RED movement event start time equals to the above YELLOW start time and end with desired green end time + desired yellow duration +  desired red clearance.
             * **/
            else if (cur_movement_state_ref.state_time_speed.front().event_state == signal_phase_and_timing::movement_phase_state::protected_clearance) // YELLOW
            {
                // Updating the current YELLOW movement event
                uint16_t spat_current_yellow_duration = tsc_state_ptr->get_signal_group_state_map().at(cur_movement_state_ref.signal_group).yellow_duration;
                uint64_t calculated_yellow_end_time_epoch = cur_movement_state_ref.state_time_speed.front().timing.get_epoch_start_time() + spat_current_yellow_duration;
                cur_movement_state_ref.state_time_speed.front().timing.set_min_end_time(calculated_yellow_end_time_epoch);

                // Add RED movement event
                signal_phase_and_timing::movement_event red_movement_event;
                uint64_t calculated_red_end_time_epoch = desired_green_end_time_epoch + desired_yellow_duration + desired_red_clearance;
                populate_movement_event(red_movement_event, signal_phase_and_timing::movement_phase_state::stop_and_remain, calculated_yellow_end_time_epoch, calculated_red_end_time_epoch);
                cur_movement_state_ref.state_time_speed.push_back(red_movement_event);
            }
            else
            {
                SPDLOG_ERROR("SPAT current movement event has to be red or yellow if its signal group id is not in desired signal group ids!");
                return;
            }
        }
    }

    void monitor_desired_phase_plan::process_second_onward_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, const std::shared_ptr<tsc_state> tsc_state_ptr)
    {
        auto desired_green_signal_group_ids = desired_sg_green_timing.signal_groups;
        auto desired_green_start_time_epoch = desired_sg_green_timing.start_time;
        auto desired_green_end_time_epoch = desired_sg_green_timing.end_time;

        // Assuming signal groups(e.g [1,5]) within the same movement group(e.g. A) has the same red_clearance and yellow_duration
        uint16_t desired_yellow_duration = tsc_state_ptr->get_signal_group_state_map().at(desired_green_signal_group_ids.front()).yellow_duration;
        uint16_t desired_red_clearance = tsc_state_ptr->get_signal_group_state_map().at(desired_green_signal_group_ids.front()).red_clearance;

        /***
         * If current signal group id is in the current desired phase plan signal group ids,
         * - Updating the last RED movement event from movement event list. Updating the last RED movement event end time equals to desired green start time.
         * - Adding GREEN event state start time to desired green start time and end time equals to desired green end time.
         * - Adding YELLOW event state start time to desired green end time and end time euqals to start time + yellow duration.
         * - Adding RED event state start time to the above YELLO end time and end time euqals to start time + red clearance.
         **/
        if (std::count(desired_green_signal_group_ids.begin(), desired_green_signal_group_ids.end(), cur_movement_state_ref.signal_group))
        {
            if (cur_movement_state_ref.state_time_speed.back().event_state != signal_phase_and_timing::movement_phase_state::stop_and_remain)
            {
                SPDLOG_ERROR("The last movement event has to be red to turn green again");
                return;
            }

            //  Updating the last RED movement end time
            cur_movement_state_ref.state_time_speed.back().timing.set_min_end_time(desired_green_start_time_epoch);

            // Append GREEN, YELLOW, and RED movement events
            append_full_green_yellow_red_phases_by_desired_green(cur_movement_state_ref, desired_sg_green_timing, desired_yellow_duration, desired_red_clearance);
        }
        else
        {
            /***
             * If current signal group id is NOT in the current desired phase plan signal group ids,
             ***/
            if (cur_movement_state_ref.state_time_speed.back().event_state == signal_phase_and_timing::movement_phase_state::stop_and_remain) // RED
            {
                // Updating the current RED event state end time equals to desired green phase end time + desired yellow duration + desired red clearance
                cur_movement_state_ref.state_time_speed.back().timing.set_min_end_time(desired_green_end_time_epoch + desired_yellow_duration + desired_red_clearance);
            }
            else
            {
                SPDLOG_ERROR("For processing future movement events, the movement event has to be red if its signal group is not in the desired signal groups.");
                return;
            }
        }
    }

    void monitor_desired_phase_plan::populate_movement_event(signal_phase_and_timing::movement_event &movemnet_event_to_populate, signal_phase_and_timing::movement_phase_state phase_state, uint64_t start_time_epoch, uint64_t end_time_epoch) const
    {
        movemnet_event_to_populate.event_state = phase_state;
        movemnet_event_to_populate.timing.set_start_time(start_time_epoch);
        movemnet_event_to_populate.timing.set_min_end_time(end_time_epoch);
    }

    void monitor_desired_phase_plan::append_full_green_yellow_red_phases_by_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, uint16_t desired_yellow_duration, uint16_t desired_red_clearance) const
    {
        auto desired_green_start_time_epoch = desired_sg_green_timing.start_time;
        auto desired_green_end_time_epoch = desired_sg_green_timing.end_time;

        // Add GREEN movement event
        signal_phase_and_timing::movement_event green_movement_event;
        populate_movement_event(green_movement_event, signal_phase_and_timing::movement_phase_state::protected_movement_allowed, desired_green_start_time_epoch, desired_green_end_time_epoch);
        cur_movement_state_ref.state_time_speed.push_back(green_movement_event);

        // Add YELLOW movement event
        signal_phase_and_timing::movement_event yellow_movement_event;
        uint64_t calculated_yellow_end_time_epoch = desired_green_end_time_epoch + desired_yellow_duration;
        populate_movement_event(yellow_movement_event, signal_phase_and_timing::movement_phase_state::protected_clearance, desired_green_end_time_epoch, calculated_yellow_end_time_epoch);
        cur_movement_state_ref.state_time_speed.push_back(yellow_movement_event);

        // Add RED movement event
        signal_phase_and_timing::movement_event red_movement_event;
        uint64_t calculated_red_end_time_epoch = calculated_yellow_end_time_epoch + desired_red_clearance;
        populate_movement_event(red_movement_event, signal_phase_and_timing::movement_phase_state::stop_and_remain, calculated_yellow_end_time_epoch, calculated_red_end_time_epoch);
        cur_movement_state_ref.state_time_speed.push_back(red_movement_event);
    }

}