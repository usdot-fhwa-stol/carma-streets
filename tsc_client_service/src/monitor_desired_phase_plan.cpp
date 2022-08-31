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
                    throw monitor_desired_phase_plan_exception("Movement event list has more than one events, not usable when adding future movement events. Associated with Signal Group: " + std::to_string(current_signal_group_id) );
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

    void monitor_desired_phase_plan::process_first_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, const std::shared_ptr<tsc_state> tsc_state_ptr) const
    {
        auto desired_green_signal_group_ids = desired_sg_green_timing.signal_groups;
        auto desired_green_start_time_epoch = desired_sg_green_timing.start_time;
        auto desired_green_end_time_epoch = desired_sg_green_timing.end_time;

        

        // If SPAT Current signal group id is in the current desired phase plan signal group ids, process desired green event for the SPAT 
        // current movement event
        int current_signal_group_id = cur_movement_state_ref.signal_group;
        // Get yellow change and red clearance time interval for signal group in movement group with the largest combination
        // of yellow change and red clearance time interval.
        int signal_group_with_largest_clearance = find_max_desired_yellow_duration_red_clearance_pair(desired_sg_green_timing.signal_groups, tsc_state_ptr);
        int desired_yellow_duration = tsc_state_ptr->get_signal_group_state_map()[signal_group_with_largest_clearance].yellow_duration;
        int desired_red_clearance = tsc_state_ptr->get_signal_group_state_map()[signal_group_with_largest_clearance].red_clearance;
        SPDLOG_DEBUG("process signal group = {0} \t AND First desired sg ids = [{1} , {2}]", 
                    current_signal_group_id, desired_green_signal_group_ids.front(), 
                    desired_green_signal_group_ids.back());
        if (std::count(desired_green_signal_group_ids.begin(), desired_green_signal_group_ids.end(), current_signal_group_id))
        {
            /**
             * If the SPAT current movement event is GREEN event state,
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
                populate_movement_event(yellow_movement_event, 
                                        signal_phase_and_timing::movement_phase_state::protected_clearance, 
                                        desired_green_end_time_epoch, 
                                        calculated_yellow_end_time_epoch);
                cur_movement_state_ref.state_time_speed.push_back(yellow_movement_event);

                // Add RED movement event
                signal_phase_and_timing::movement_event red_movement_event;
                populate_movement_event(red_movement_event, 
                                        signal_phase_and_timing::movement_phase_state::stop_and_remain, 
                                        calculated_yellow_end_time_epoch,
                                        calculated_yellow_end_time_epoch + desired_red_clearance);
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
                append_full_green_yellow_red_phases_by_desired_green(cur_movement_state_ref, 
                                                                    desired_sg_green_timing, 
                                                                    desired_yellow_duration, 
                                                                    desired_red_clearance);
            }
            else
            {
                throw monitor_desired_phase_plan_exception("SPAT current movement event has to be green or red event state!");
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
                cur_movement_state_ref.state_time_speed.front().timing.set_min_end_time(desired_green_end_time_epoch + 
                                                                                        desired_yellow_duration + 
                                                                                        desired_red_clearance);
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
                int spat_current_yellow_duration = tsc_state_ptr->get_signal_group_state_map()[current_signal_group_id].yellow_duration;
                uint64_t calculated_yellow_end_time_epoch = cur_movement_state_ref.state_time_speed.front().timing.get_epoch_start_time() 
                                                            + spat_current_yellow_duration;
                cur_movement_state_ref.state_time_speed.front().timing.set_min_end_time(calculated_yellow_end_time_epoch);

                // Add RED movement event
                signal_phase_and_timing::movement_event red_movement_event;
                uint64_t calculated_red_end_time_epoch = desired_green_end_time_epoch + desired_yellow_duration + desired_red_clearance;
                populate_movement_event(red_movement_event, 
                                        signal_phase_and_timing::movement_phase_state::stop_and_remain, 
                                        calculated_yellow_end_time_epoch, 
                                        calculated_red_end_time_epoch);
                cur_movement_state_ref.state_time_speed.push_back(red_movement_event);
            }
            else
            {
                throw monitor_desired_phase_plan_exception("SPAT current movement event has to be red or yellow if its signal group id is not in desired signal group ids!");
            }
        }
    }

    void monitor_desired_phase_plan::process_second_onward_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                                                        const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, 
                                                                        const std::shared_ptr<tsc_state> tsc_state_ptr) const
    {
        auto desired_green_signal_group_ids = desired_sg_green_timing.signal_groups;
        auto desired_green_start_time_epoch = desired_sg_green_timing.start_time;
        auto desired_green_end_time_epoch = desired_sg_green_timing.end_time;

        
        /***
         * If current signal group id is in the current desired phase plan signal group ids,
         * - Updating the last RED movement event from movement event list. Updating the last RED movement event end time equals to desired green start time.
         * - Adding GREEN event state start time to desired green start time and end time equals to desired green end time.
         * - Adding YELLOW event state start time to desired green end time and end time euqals to start time + yellow duration.
         * - Adding RED event state start time to the above YELLO end time and end time euqals to start time + red clearance.
         **/
        int current_signal_group_id = cur_movement_state_ref.signal_group;
        // Get yellow change and red clearance time interval for signal group in movement group with the largest combination
        // of yellow change and red clearance time interval.
        int signal_group_with_largest_clearance = find_max_desired_yellow_duration_red_clearance_pair(desired_sg_green_timing.signal_groups, tsc_state_ptr);
        int desired_yellow_duration = tsc_state_ptr->get_signal_group_state_map()[signal_group_with_largest_clearance].yellow_duration;
        int desired_red_clearance = tsc_state_ptr->get_signal_group_state_map()[signal_group_with_largest_clearance].red_clearance;
        SPDLOG_DEBUG("process signal group = {0} \t AND second and onwards desired sg ids = [{1} , {2}]", 
                    current_signal_group_id, 
                    desired_green_signal_group_ids.front(), 
                    desired_green_signal_group_ids.back());
        if (std::count(desired_green_signal_group_ids.begin(), desired_green_signal_group_ids.end(), current_signal_group_id))
        {
            if (cur_movement_state_ref.state_time_speed.back().event_state != signal_phase_and_timing::movement_phase_state::stop_and_remain)
            {
                throw monitor_desired_phase_plan_exception("The last movement event has to be red to turn green again");
            }

            //  Updating the last RED movement end time
            cur_movement_state_ref.state_time_speed.back().timing.set_min_end_time(desired_green_start_time_epoch);

            // Append GREEN, YELLOW, and RED movement events
            append_full_green_yellow_red_phases_by_desired_green(cur_movement_state_ref, 
                                                                desired_sg_green_timing, 
                                                                desired_yellow_duration, 
                                                                desired_red_clearance);
        }
        else
        {
            /***
             * If current signal group id is NOT in the current desired phase plan signal group ids,
             ***/
            if (cur_movement_state_ref.state_time_speed.back().event_state == signal_phase_and_timing::movement_phase_state::stop_and_remain) // RED
            {
                // Updating the current RED event state end time equals to desired green phase end time + desired yellow duration + desired red clearance
                cur_movement_state_ref.state_time_speed.back().timing.set_min_end_time(desired_green_end_time_epoch 
                                                                                        + desired_yellow_duration 
                                                                                        + desired_red_clearance);
            }
            else
            {
                throw monitor_desired_phase_plan_exception("For processing future movement events, the movement event has to be red if its signal group is not in the desired signal groups.");
            }
        }
    }

    void monitor_desired_phase_plan::populate_movement_event(signal_phase_and_timing::movement_event &movemnet_event_to_populate, 
                                                            const signal_phase_and_timing::movement_phase_state &phase_state, 
                                                            const uint64_t start_time_epoch, 
                                                            const uint64_t end_time_epoch) const
    {
        movemnet_event_to_populate.event_state = phase_state;
        movemnet_event_to_populate.timing.set_start_time(start_time_epoch);
        movemnet_event_to_populate.timing.set_min_end_time(end_time_epoch);
    }

    void monitor_desired_phase_plan::append_full_green_yellow_red_phases_by_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                                                                        const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing, 
                                                                                        int desired_yellow_duration, 
                                                                                        int desired_red_clearance) const
    {
        auto desired_green_start_time_epoch = desired_sg_green_timing.start_time;
        auto desired_green_end_time_epoch = desired_sg_green_timing.end_time;

        // Add GREEN movement event
        signal_phase_and_timing::movement_event green_movement_event;
        populate_movement_event(green_movement_event, 
                                signal_phase_and_timing::movement_phase_state::protected_movement_allowed, 
                                desired_green_start_time_epoch, desired_green_end_time_epoch);
        cur_movement_state_ref.state_time_speed.push_back(green_movement_event);

        // Add YELLOW movement event
        signal_phase_and_timing::movement_event yellow_movement_event;
        uint64_t calculated_yellow_end_time_epoch = desired_green_end_time_epoch + desired_yellow_duration;
        populate_movement_event(yellow_movement_event,
                                signal_phase_and_timing::movement_phase_state::protected_clearance, 
                                desired_green_end_time_epoch, calculated_yellow_end_time_epoch);
        cur_movement_state_ref.state_time_speed.push_back(yellow_movement_event);

        // Add RED movement event
        signal_phase_and_timing::movement_event red_movement_event;
        uint64_t calculated_red_end_time_epoch = calculated_yellow_end_time_epoch + desired_red_clearance;
        populate_movement_event(red_movement_event, 
                                signal_phase_and_timing::movement_phase_state::stop_and_remain, 
                                calculated_yellow_end_time_epoch,
                                calculated_red_end_time_epoch);
        cur_movement_state_ref.state_time_speed.push_back(red_movement_event);
    }

    int monitor_desired_phase_plan::find_max_desired_yellow_duration_red_clearance_pair(std::vector<int> desired_signal_groups, const std::shared_ptr<tsc_state> tsc_state_ptr) const
    {
        int return_sg_id = desired_signal_groups.front();
        int total = 0;
        for (auto sg : desired_signal_groups)
        {
            int local_total = tsc_state_ptr->get_signal_group_state_map()[sg].red_clearance + tsc_state_ptr->get_signal_group_state_map()[sg].yellow_duration;
            if (local_total > total)
            {
                total = local_total;
                return_sg_id = sg;
            }
        }
        return return_sg_id;
    }

}