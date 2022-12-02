#include "streets_desired_phase_plan_generator.h"

namespace streets_signal_optimization {

    std::vector<streets_desired_phase_plan::streets_desired_phase_plan> 
                        streets_desired_phase_plan_generator::generate_desire_phase_plan_list
                            (const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                            std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles,
                            signal_phase_and_timing::intersection_state &intersection_state,
                            const std::shared_ptr<streets_signal_optimization::movement_groups> &move_groups, 
                            const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> &tsc_config_ptr) {

        std::vector<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan_list;

        /**
         * First, check if all signal groups included in the movement group list exist in the intersection_info pointer
         * (i.e., each signal group is connected to at least one link lanelet).
        */
        verify_movement_group_list(intersection_info_ptr, move_groups);

        /** If the configurable parameters are not set, set them to their default values.
         * Default values:
         * - initial_green_buffer = 2,000 ms
         * - final_green_buffer = 2,000 ms
         * - et_inaccuracy_buffer = 2,000 ms
         * - queue_max_time_headway = 3,000 ms
         * - so_radius = 200 meter
         * - min_green = 50,000 ms
         * - max_green = 120,000 ms
         * - desired_future_move_group_count = 1
         */
        if (!is_configured) {
            SPDLOG_WARN("set_configuration was not called. Setting configuration to default!");
            set_configuration(2000, 2000, 2000, 3000, 200, 5000, 120000, 1);
        }
        
        /** 
         * Convert the provided intersection_state to a desired_phase_plan and name it base_desired_phase_plan.
         * If base_desired_phase_plan is empty, then throw exception.
         * If the number of fixed movement group in base_desired_phase_plan is greater than the desired number of fixed 
         *     movement groups, then there is no need for adding a new movement group to base_desired_phase_plan. Thus, 
         *     return an empty desired_phase_plan_list. Otherwise, find the start time of the TBD area and continue.
        */
        streets_desired_phase_plan::streets_desired_phase_plan base_desired_phase_plan = convert_spat_to_dpp(intersection_state, move_groups);
        if (base_desired_phase_plan.desired_phase_plan.empty()) {
            throw streets_desired_phase_plan_generator_exception("No green movement group is found in the spat. base_desired_phase_plan is empty!");
        }
        if (base_desired_phase_plan.desired_phase_plan.size() > config.desired_future_move_group_count) {
            SPDLOG_DEBUG("The number of fixed future movement groups in the modified spat ({0}) is greater than the desired number of fixed future movement groups ({1})!", base_desired_phase_plan.desired_phase_plan.size(), config.desired_future_move_group_count);
            return desired_phase_plan_list;
        }
        /**
         * The provided spat have only a configurable number of fixed future movement groups. Any time area after the fixed future movement
         *     groups is defined as the to-be-determined (TBD) area. Basically, the TBD area starts from the end time of the red clearance of
         *     the last fixed greens. If a vehicle's estimated entering time (ET) is within the TBD area (after the end 
         *     time of the last movement_state), the vehicle will be considered in the queue calculation.
        */
        uint64_t tbd_start = find_tbd_start_time(base_desired_phase_plan, tsc_config_ptr);


        /** If the vehicle map is empty, return an empty desired phase plan list. */
        if ( vehicles.empty() ) {
            SPDLOG_DEBUG("No vehicles to schedule.");
            return desired_phase_plan_list;
        }
        
        /** 
         * Create a local spat pointer. Check if the scheduler pointer is configured. Set the spat in the scheduler pointer.
         * Then, schedule all vehicles and get the intersection_scheduler pointer 
         * */
        auto spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
        spat_ptr->set_intersection(intersection_state);
        if (!scheduler_ptr) {
            SPDLOG_DEBUG("scheduler_ptr is not configured!");
            configure_scheduler(intersection_info_ptr);
        }
        scheduler_ptr->set_spat(spat_ptr);
        std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> sched_ptr = get_schedule_plan(vehicles);


        /** Then, add the schedule plans for those EVs that are within the SO area to a separate signalized_intersection_schedule object.
         *  If the new list is empty (i.e., there is no EV in the SO area), return an empty desired phase plan list.
         */
        streets_vehicle_scheduler::signalized_intersection_schedule ev_schedules_within_so = get_ev_schedules_within_so(sched_ptr, vehicles);
        if (ev_schedules_within_so.vehicle_schedules.empty()){
            SPDLOG_DEBUG("No EV within the SO area.");
            return desired_phase_plan_list;
        }
        
        /** Then, seperate EV schedules that have estimated entering times (ETs) within TBD area based on their entry lane. 
         *  The list of EV schedules for each entry lane is sorted based on the estimated ET. 
         *  If the schedules in TBD per entry lane map is empty (i.e., there is no EV with an estimated ET within TBD area), 
         *      return an empty desired phase plan list.
         */
        std::unordered_map<int, std::list<streets_vehicle_scheduler::signalized_vehicle_schedule>> schedules_in_tbd = get_schedules_in_tbd_per_lane(ev_schedules_within_so, intersection_info_ptr, tbd_start);            
        if (schedules_in_tbd.empty()){
            SPDLOG_DEBUG("No EV within the TBD area.");
            return desired_phase_plan_list;
        }

        /** For each entry lane, find the last vehicle in the queue, calculate the queue dissipation time, and finally, 
         *      estimate the end time of the required green for dissipation the queue from the subject entry lane.
         *  If the green end time per entry lane map is empty, throw exception (there are EVs with estimated ET within TVD area,
         *      but there is no green considered for any vehicle!) and return an empty desired phase plan list.
         */
        std::unordered_map<int, uint64_t> green_end_per_entry_lane = get_green_end_per_entry_lane(schedules_in_tbd, tbd_start);
        if (green_end_per_entry_lane.empty()) {               
            SPDLOG_WARN("Vehicles schedules in TBD is not empty, green_end_per_entry_lane map is empty!");
            return desired_phase_plan_list;
        }

        /** For each possible movement group, first, check if the movement group can be added to the base desired phase plan,
         *      (check if the subject movement group has a common signal group with the last movement group in the base
         *      desired phase plan list or not!). If the movement group can be added to the base desired phase plan, for each
         *      entry lane included in the subject movement group, add a new desired phase plan to the desired phase plan list
         *      where the subject movement group is added to the base desired phsae plan with the green end time calculated for
         *      the entry lane.
         *  If the desired phase plan list is empty, it means that no movement group that clears a given queue can be added
         *      to the base desired phase plan.
         */
        update_desired_phase_plan_list(move_groups, green_end_per_entry_lane, base_desired_phase_plan, desired_phase_plan_list, intersection_info_ptr, tbd_start);
        if (desired_phase_plan_list.empty()) {
            SPDLOG_WARN("No movement group could be added and thus, the desired_phase_plan_list is empty!");
        }

        return desired_phase_plan_list;

    }


   void streets_desired_phase_plan_generator::verify_movement_group_list(
                    const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                    const std::shared_ptr<streets_signal_optimization::movement_groups> &move_groups) const
    {
        std::vector<int> signal_group_list;
        for (const auto &lane : intersection_info_ptr->getLinkLanelets()) {
            if ( std::find(signal_group_list.begin(), signal_group_list.end(), lane.getSignalGroupId()) == signal_group_list.end() ) {
                signal_group_list.push_back(lane.getSignalGroupId());
            }
        }
        
        for (const auto &mg : move_groups->groups) {
            if (std::find(signal_group_list.begin(), signal_group_list.end(), mg.signal_groups.first) == signal_group_list.end()) {
                throw streets_desired_phase_plan_generator_exception("Signal group " + std::to_string(mg.signal_groups.first) + 
                                                        " does not exist in intersection_info pointer!");
            }
            if (mg.signal_groups.second != 0 && 
                    std::find(signal_group_list.begin(), signal_group_list.end(), mg.signal_groups.second) == signal_group_list.end()) {
                throw streets_desired_phase_plan_generator_exception("Signal group " + std::to_string(mg.signal_groups.second) + 
                                                        " does not exist in intersection_info pointer!");
            }
        }
    }


    streets_desired_phase_plan::streets_desired_phase_plan streets_desired_phase_plan_generator::convert_spat_to_dpp(
                    signal_phase_and_timing::intersection_state &intersection_state, 
                    const std::shared_ptr<streets_signal_optimization::movement_groups> &move_groups) const {
        
        streets_desired_phase_plan::streets_desired_phase_plan base_desired_phase_plan;
        base_desired_phase_plan.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        std::vector<streets_desired_phase_plan::signal_group2green_phase_timing> mg_green_list;

        for (const auto& mg : move_groups->groups) {
            signal_phase_and_timing::movement_state move_state_1;
            signal_phase_and_timing::movement_state move_state_2;
            move_state_1 = intersection_state.get_movement(mg.signal_groups.first); 
            if (move_state_1.state_time_speed.empty()) {
                throw streets_desired_phase_plan_generator_exception("signal group " + std::to_string(move_state_1.signal_group) + " has empty movement_state list in the provided intersection_state object!");
            } 
            if (mg.signal_groups.second != 0) {
                move_state_2 = intersection_state.get_movement(mg.signal_groups.second);
                if (move_state_2.state_time_speed.empty()) {
                    throw streets_desired_phase_plan_generator_exception("signal group " + std::to_string(move_state_2.signal_group) + " has empty movement_state list in the provided intersection_state object!");
                } 
            }
                
            for (const auto& me1 : move_state_1.state_time_speed) {
                if (me1.event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed) {
                    streets_desired_phase_plan::signal_group2green_phase_timing movement_group;
                    movement_group.start_time = me1.timing.get_epoch_start_time();
                    movement_group.end_time = me1.timing.get_epoch_min_end_time();
                    movement_group.signal_groups.push_back(move_state_1.signal_group);

                    if (mg.signal_groups.second == 0) {
                        mg_green_list.push_back(movement_group);
                        continue;
                    }

                    for (const auto& me2 : move_state_2.state_time_speed) {
                        if (me2.event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed && 
                                    ((me2.timing.get_epoch_start_time() < me1.timing.get_epoch_start_time() && 
                                    me2.timing.get_epoch_min_end_time() > me1.timing.get_epoch_start_time()) ||
                                    (me2.timing.get_epoch_start_time() < me1.timing.get_epoch_min_end_time() && 
                                    me2.timing.get_epoch_min_end_time() > me1.timing.get_epoch_min_end_time()))) {
                            throw streets_desired_phase_plan_generator_exception("spat has fixed future signal groups with partially overlapping green durations!");
                        }
                        if (me2.event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed && 
                                    me2.timing.get_epoch_start_time() == me1.timing.get_epoch_start_time() && 
                                    me2.timing.get_epoch_min_end_time() == me1.timing.get_epoch_min_end_time()) {
                            
                            movement_group.signal_groups.push_back(move_state_2.signal_group);
                            mg_green_list.push_back(movement_group);
                            break;
                        }
                        else if (me2.timing.get_epoch_start_time() > me1.timing.get_epoch_start_time()) {
                            break;
                        }
                    }
                }
            }
        }
        
        for (const auto& mg_green : mg_green_list) {
            if (base_desired_phase_plan.desired_phase_plan.empty()) {
                base_desired_phase_plan.desired_phase_plan.push_back(mg_green);
            }
            else {
                for (int index = 0; index < base_desired_phase_plan.desired_phase_plan.size(); ++index) {
                    if ( (mg_green.start_time < base_desired_phase_plan.desired_phase_plan[index].start_time && 
                                mg_green.end_time > base_desired_phase_plan.desired_phase_plan[index].start_time) || 
                            (mg_green.start_time < base_desired_phase_plan.desired_phase_plan[index].end_time && 
                                mg_green.end_time > base_desired_phase_plan.desired_phase_plan[index].end_time) ) {
                        throw streets_desired_phase_plan_generator_exception("spat has future movement groups with partially overlapping green durations!");
                    }
                    if (mg_green.end_time < base_desired_phase_plan.desired_phase_plan[index].start_time) {
                        base_desired_phase_plan.desired_phase_plan.insert(base_desired_phase_plan.desired_phase_plan.begin() + index, mg_green);
                        break;
                    }
                    if (mg_green.start_time > base_desired_phase_plan.desired_phase_plan[index].end_time) {
                        base_desired_phase_plan.desired_phase_plan.insert(base_desired_phase_plan.desired_phase_plan.begin() + index + 1, mg_green);
                        break;
                    }
                }
            }
        }
        SPDLOG_DEBUG("Base desired phase plan converted from the modified spat: {0}", base_desired_phase_plan.toJson());
        
        return base_desired_phase_plan;
    }


    uint64_t streets_desired_phase_plan_generator::find_tbd_start_time(
                    const streets_desired_phase_plan::streets_desired_phase_plan &base_desired_phase_plan, 
                    const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> &tsc_config_ptr) const {
        uint64_t tbd_start = 0;
        if (base_desired_phase_plan.desired_phase_plan.empty()) {
            throw streets_desired_phase_plan_generator_exception("No green movement group is found in the spat. base_desired_phase_plan is empty!");
        }
        // find the last fixed movement group in the base_desired_phase_plan
        auto last_movement_group = base_desired_phase_plan.desired_phase_plan.back();
        uint64_t yellow_red_interval_local = 0;
        uint64_t yellow_red_interval_max = 0;
        // find the maximum (yellow_change_duration + red_clearance) for the signal groups included in the last fixed movement group
        for (const auto sg : last_movement_group.signal_groups) {
            auto sg_config = tsc_config_ptr->get_signal_group_configuration_by_sg(sg);
            if (sg != static_cast<int>(sg_config.signal_group_id)) {
                throw streets_desired_phase_plan_generator_exception("TSC_config does not have signal group " + std::to_string(sg) + "'s configuration parameters!");
            }
            yellow_red_interval_local = static_cast<uint64_t>(sg_config.yellow_change_duration + sg_config.red_clearance);
            if (yellow_red_interval_max < yellow_red_interval_local) {
                yellow_red_interval_max = yellow_red_interval_local;
            }
        }
        if (yellow_red_interval_max == 0) {
            throw streets_desired_phase_plan_generator_exception("yellow_red_interval_max that stores the maximum (yellow_change_duration + red_clearance) for the signal groups included in the last fixed movement group, is not updated!");
        }
        tbd_start = last_movement_group.end_time + yellow_red_interval_max;

        SPDLOG_DEBUG("TBD start time: {0}", tbd_start);
        return tbd_start;
    }


    void streets_desired_phase_plan_generator::configure_scheduler(
                    const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr) {
        /** configure the signalized_vehicle_scheduler pointer */
        scheduler_ptr = std::make_shared<streets_vehicle_scheduler::signalized_vehicle_scheduler>();
        scheduler_ptr->set_intersection_info(intersection_info_ptr);
        scheduler_ptr->set_initial_green_buffer(config.initial_green_buffer);
        scheduler_ptr->set_final_green_buffer(config.final_green_buffer);
    }


    std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> streets_desired_phase_plan_generator::get_schedule_plan(
                    std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles) const {

        /** configure the signalized_intersection_schedule pointer */
        std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> sched_ptr;
        sched_ptr = std::make_shared<streets_vehicle_scheduler::signalized_intersection_schedule>();
        sched_ptr->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        
        /** schedule vehicles */
        scheduler_ptr->schedule_vehicles(vehicles, sched_ptr);
        return sched_ptr;
    }


    streets_vehicle_scheduler::signalized_intersection_schedule streets_desired_phase_plan_generator::get_ev_schedules_within_so(
                    const std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> &sched_ptr, 
                    const std::unordered_map<std::string,streets_vehicles::vehicle> &vehicles) const {
        
        auto schedule_ptr = std::dynamic_pointer_cast<streets_vehicle_scheduler::signalized_intersection_schedule> (sched_ptr);
        streets_vehicle_scheduler::signalized_intersection_schedule ev_schedules_within_so;
        for (const auto& veh_sched : schedule_ptr->vehicle_schedules) {
            if (veh_sched.state == streets_vehicles::vehicle_state::EV && vehicles.at(veh_sched.v_id)._cur_distance <= config.so_radius) {
                ev_schedules_within_so.vehicle_schedules.push_back(veh_sched);
            }
        }
        return ev_schedules_within_so;
    }


    std::unordered_map<int, std::list<streets_vehicle_scheduler::signalized_vehicle_schedule>> 
                streets_desired_phase_plan_generator::get_schedules_in_tbd_per_lane(
                    const streets_vehicle_scheduler::signalized_intersection_schedule &ev_schedules_within_so, 
                    const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                    const uint64_t &tbd_start) const {

        std::unordered_map<int, std::list<streets_vehicle_scheduler::signalized_vehicle_schedule>> schedules_per_lane;
        for ( const auto &entry_lane_obj : intersection_info_ptr->getEntryLanelets() ) {
            std::list<streets_vehicle_scheduler::signalized_vehicle_schedule> schedules_in_lane;
            for ( const auto &ev_sched : ev_schedules_within_so.vehicle_schedules ) {
                if (ev_sched.state == streets_vehicles::vehicle_state::EV && ev_sched.entry_lane == entry_lane_obj.getId() && ev_sched.et >= tbd_start) {
                    SPDLOG_DEBUG("Adding schedule for vehicle {0} to EV schedule list in entry lane {1}", ev_sched.v_id, ev_sched.entry_lane);
                    schedules_in_lane.push_back(ev_sched);
                }
            }
            if ( !schedules_in_lane.empty()) {
                schedules_in_lane.sort(streets_vehicle_scheduler::et_comparator);
                schedules_per_lane.try_emplace( entry_lane_obj.getId(), schedules_in_lane );
            }
            else {
                SPDLOG_DEBUG("No EVs in lane {0}.", static_cast<int>(entry_lane_obj.getId()) );
            }
        }
        return schedules_per_lane;
    }


    std::unordered_map<int, uint64_t> streets_desired_phase_plan_generator::get_green_end_per_entry_lane(
                    const std::unordered_map<int, std::list<streets_vehicle_scheduler::signalized_vehicle_schedule>> &schedules_in_tbd, 
                    const uint64_t &tbd_start) const {
        
        std::unordered_map<int, uint64_t> green_end_per_entry_lane; 
        
        for (const auto &[entry_lane_id, evs_in_lane] : schedules_in_tbd) {
        
            SPDLOG_DEBUG("Calculate green end time for entry lane {0}", entry_lane_id);

            // Get last vehicle in queue
            if (!evs_in_lane.empty()) {
                
                streets_vehicle_scheduler::signalized_vehicle_schedule last_ev_in_queue = get_last_vehicle_in_queue(evs_in_lane);
                
                uint64_t et_rounded = uint64_t(std::ceil(double(last_ev_in_queue.et)/100.0) * 100);
                uint64_t green_end_compared_with_min_green = std::max(et_rounded + config.et_inaccuracy_buffer + config.final_green_buffer, tbd_start + config.min_green);
                uint64_t green_end = std::min(green_end_compared_with_min_green, tbd_start + config.max_green);
                // in case the green_end is overwritten by max_green, check if there is any ET within the new green duration.
                // if not, skip this entry lane, assuming there is no queue that can be served.
                if (green_end < green_end_compared_with_min_green && evs_in_lane.front().et >= green_end - config.final_green_buffer) {
                    SPDLOG_WARN("The required green duration for dissipation the queue in entry lane " + 
                            std::to_string(entry_lane_id) + 
                            " is overwritten by max_green which cannot serve any vehicle! green_end without considering max_green: " + 
                            std::to_string(green_end_compared_with_min_green) + 
                            ", green_end considering max_green: " + std::to_string(green_end) + 
                            ", first EV's entering time: " + std::to_string(evs_in_lane.front().et) + ".");
                }
                else 
                {
                    green_end_per_entry_lane.try_emplace(entry_lane_id, green_end);   
                }
            }
            else {
                SPDLOG_DEBUG("There are no EVs in entry lane {0} that is within the SO area and has an estimated ET after the start time of the TBD area", entry_lane_id);
            }
        }

        return green_end_per_entry_lane;
    }


    streets_vehicle_scheduler::signalized_vehicle_schedule streets_desired_phase_plan_generator::get_last_vehicle_in_queue(
                    const std::list<streets_vehicle_scheduler::signalized_vehicle_schedule> &evs_in_lane) const {

        bool is_last_vehicle_in_queue_found = false;
        streets_vehicle_scheduler::signalized_vehicle_schedule prev_ev_sched;
        streets_vehicle_scheduler::signalized_vehicle_schedule last_ev_in_queue;
        for (const auto &ev_sched : evs_in_lane) {
            if (ev_sched.v_id != evs_in_lane.front().v_id && ev_sched.et == ev_sched.eet && (ev_sched.et - prev_ev_sched.et >= config.queue_max_time_headway)) {
                
                is_last_vehicle_in_queue_found = true;
                last_ev_in_queue = prev_ev_sched;
            }
            prev_ev_sched = ev_sched;
        }
        if (!is_last_vehicle_in_queue_found) {
            last_ev_in_queue = evs_in_lane.back();
        }
        return last_ev_in_queue;
    }


    void streets_desired_phase_plan_generator::update_desired_phase_plan_list(
                    const std::shared_ptr<streets_signal_optimization::movement_groups> &move_groups, 
                    const std::unordered_map<int, uint64_t> &green_end_per_entry_lane, 
                    const streets_desired_phase_plan::streets_desired_phase_plan &base_desired_phase_plan, 
                    std::vector<streets_desired_phase_plan::streets_desired_phase_plan> &desired_phase_plan_list, 
                    const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                    const uint64_t &tbd_start) {
        
        /** If the base_desired_phase_plan, it means that either the base_desired_phase_plan is not created based on the spat,
         * or the spat does not have any fixed movement group (even the current active phase timing) in it.
        */
        if (base_desired_phase_plan.desired_phase_plan.empty()) {
            throw streets_desired_phase_plan_generator_exception("The base desired phase plan is empty!");
        }
        /** If the signal_group_entry_lane_mapping is empty, call create_signal_group_entry_lane_mapping() function to find the mapping.
         *  This mapping maps signal group ids and entry lane ids.
        */
        if (signal_group_entry_lane_mapping.empty()) {
            SPDLOG_DEBUG("The signal_group_entry_lane_mapping is empty!");
            create_signal_group_entry_lane_mapping(intersection_info_ptr);
        }
        /** 
         * Loop through all candidate movement groups, check if it can be added to the base_desired_phase_plan, and if yes, 
         * add it to the base_desired_phase_plan with all different possible green duration options.
        */
        for (const auto &move_group : move_groups->groups) {
            
            /** Check if the movement group can be added to the end of the base desired phase plan */
            bool can_mg_be_added = true;
            for (const auto &signal_group : base_desired_phase_plan.desired_phase_plan.back().signal_groups) {
                if ( signal_group == move_group.signal_groups.first || (move_group.signal_groups.second != 0 && signal_group == move_group.signal_groups.second) ) {
                    can_mg_be_added = false;
                    break;
                }
            }
            if (can_mg_be_added) {
                
                /**
                 * Loop through the entry lanes connected to the first signal group. For each entry lane, add the movement group 
                 * to the end of the base desired phase plan with the required green duration calculated for this entry lane. 
                */
                for (const auto &entry_lane : signal_group_entry_lane_mapping.at(move_group.signal_groups.first)) {
                    if (green_end_per_entry_lane.find(entry_lane) != green_end_per_entry_lane.end()) {
                        desired_phase_plan_list.push_back(base_desired_phase_plan);
                        streets_desired_phase_plan::signal_group2green_phase_timing mg_timing;
                        mg_timing.start_time = tbd_start;
                        mg_timing.end_time = green_end_per_entry_lane.at(entry_lane);
                        mg_timing.signal_groups.push_back(move_group.signal_groups.first);
                        if (move_group.signal_groups.second != 0) {
                            mg_timing.signal_groups.push_back(move_group.signal_groups.second);
                        }
                        desired_phase_plan_list.back().desired_phase_plan.push_back(mg_timing);
                    }
                }

                /**
                 * If the movement group contains 2 signal groups, loop through the entry lanes connected to the second signal group. 
                 * For each entry lane, add the movement group to the end of the base desired phase plan with the required green 
                 * duration calculated for this entry lane. 
                */
                if (move_group.signal_groups.second != 0) {
                    for (const auto &entry_lane : signal_group_entry_lane_mapping.at(move_group.signal_groups.second)) {
                        if (green_end_per_entry_lane.find(entry_lane) != green_end_per_entry_lane.end()) {
                            desired_phase_plan_list.push_back(base_desired_phase_plan);
                            streets_desired_phase_plan::signal_group2green_phase_timing mg_timing;
                            mg_timing.start_time = tbd_start;
                            mg_timing.end_time = green_end_per_entry_lane.at(entry_lane);
                            mg_timing.signal_groups.push_back(move_group.signal_groups.first);
                            mg_timing.signal_groups.push_back(move_group.signal_groups.second);
                            desired_phase_plan_list.back().desired_phase_plan.push_back(mg_timing);
                        }
                    }
                }

            }
            else {
                SPDLOG_DEBUG("movement group {0} cannot be added to base_desired_phase_plan since this movement group includes a common signal group with the last fixed movement group in the modified spat!", move_group.name);
            }
        }
    }


    void streets_desired_phase_plan_generator::create_signal_group_entry_lane_mapping(
                    const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr) {
        
        signal_group_entry_lane_mapping.clear();
        for ( const auto &lane : intersection_info_ptr->getEntryLanelets() ) {
            uint8_t signal_group = find_signal_group_for_entry_lane(intersection_info_ptr, lane);
            SPDLOG_DEBUG("Creating signal group to entry lane mapping - entry lane id = {0}, signal group id = {1}", lane.getId(), signal_group);
            if (signal_group_entry_lane_mapping.find(signal_group) == signal_group_entry_lane_mapping.end()) {
                std::vector<int> lane_ids;
                lane_ids.push_back(lane.getId());
                signal_group_entry_lane_mapping.try_emplace(signal_group, lane_ids);
            }
            else {
                signal_group_entry_lane_mapping.at(signal_group).push_back(lane.getId());
            }
        }
    }


    uint8_t streets_desired_phase_plan_generator::find_signal_group_for_entry_lane(
                    const std::shared_ptr<OpenAPI::OAIIntersection_info> &intersection_info_ptr, 
                    const OpenAPI::OAILanelet_info &entry_lane_info) const {
        
        // check if all links connected to the entry lane have the same signal ids or not!
        uint8_t signal_group_id = 0;
        bool first_link_visited = false;
        auto connection_lanelet_ids = entry_lane_info.getConnectingLaneletIds();
        for ( const auto &lane : intersection_info_ptr->getLinkLanelets() ) {
            // check if the link lanelet's id (lane.getId()) is included in the list of connecting lanelet ids of the received entry lanelet.
            if ( std::find(connection_lanelet_ids.begin(), connection_lanelet_ids.end(), lane.getId()) != connection_lanelet_ids.end() ) {
                if ( !lane.getSignalGroupId() ) {
                    throw streets_desired_phase_plan_generator_exception("The connection link lanelet does not have a group_id!");
                }
                if (first_link_visited && lane.getSignalGroupId() != signal_group_id){
                    throw streets_desired_phase_plan_generator_exception("The link lanelets connected to the entry lane have different signal_group_ids! The streets_desired_phase_plan_generator is only capable of understanding intersection where all connection lanes from a single entry lane share a signal_group_id!");
                }
                if (!first_link_visited) {
                    signal_group_id = lane.getSignalGroupId();
                    first_link_visited = true;
                }
            }
        }
        if (signal_group_id == 0) {
            throw streets_desired_phase_plan_generator_exception("Could not find signal group id for entry lane " + 
                                    std::to_string(entry_lane_info.getId()) + "!");
        }
        return signal_group_id;
    }


    void streets_desired_phase_plan_generator::set_configuration(const uint64_t _initial_green_buffer, 
                                                        const uint64_t _final_green_buffer, 
                                                        const uint64_t _et_inaccuracy_buffer, 
                                                        const uint64_t _queue_max_time_headway,
                                                        const double _so_radius,
                                                        const uint64_t _min_green,
                                                        const uint64_t _max_green,
                                                        const uint8_t _desired_future_move_group_count) {
        config.initial_green_buffer = _initial_green_buffer;
        config.final_green_buffer = _final_green_buffer; 
        config.et_inaccuracy_buffer = _et_inaccuracy_buffer; 
        config.queue_max_time_headway = _queue_max_time_headway;
        config.so_radius = _so_radius;
        config.min_green = _min_green;
        config.max_green = _max_green;
        config.desired_future_move_group_count = _desired_future_move_group_count;
        is_configured = true;
        SPDLOG_DEBUG("Configuration parameter - initial_green_buffer: {0}", config.initial_green_buffer);
        SPDLOG_DEBUG("Configuration parameter - final_green_buffer: {0}", config.final_green_buffer);
        SPDLOG_DEBUG("Configuration parameter - et_inaccuracy_buffer: {0}", config.et_inaccuracy_buffer);
        SPDLOG_DEBUG("Configuration parameter - queue_max_time_headway: {0}", config.queue_max_time_headway);
        SPDLOG_DEBUG("Configuration parameter - so_radius: {0}", config.so_radius);
        SPDLOG_DEBUG("Configuration parameter - min_green: {0}", config.min_green);
        SPDLOG_DEBUG("Configuration parameter - max_green: {0}", config.max_green);
        SPDLOG_DEBUG("Configuration parameter - desired_future_move_group_count: {0}", config.desired_future_move_group_count);
    }

    void streets_desired_phase_plan_generator::set_initial_green_buffer(const uint64_t buffer){
        config.initial_green_buffer = buffer;
    }

    uint64_t streets_desired_phase_plan_generator::get_initial_green_buffer() const {
        return config.initial_green_buffer;
    }

    void streets_desired_phase_plan_generator::set_final_green_buffer(const uint64_t buffer){
        config.final_green_buffer = buffer;
    }

    uint64_t streets_desired_phase_plan_generator::get_final_green_buffer() const {
        return config.final_green_buffer;
    }

    void streets_desired_phase_plan_generator::set_et_inaccuracy_buffer(const uint64_t buffer){
        config.et_inaccuracy_buffer = buffer;
    }

    uint64_t streets_desired_phase_plan_generator::get_et_inaccuracy_buffer() const {
        return config.et_inaccuracy_buffer;
    }

    void streets_desired_phase_plan_generator::set_queue_max_time_headway(const uint64_t buffer){
        config.queue_max_time_headway = buffer;
    }

    uint64_t streets_desired_phase_plan_generator::get_queue_max_time_headway() const {
        return config.queue_max_time_headway;
    }

    void streets_desired_phase_plan_generator::set_so_radius(const double radius){
        config.so_radius = radius;
    }

    double streets_desired_phase_plan_generator::get_so_radius() const {
        return config.so_radius;
    }

    void streets_desired_phase_plan_generator::set_min_green(const uint64_t _min_green){
        config.min_green = _min_green;
    }

    uint64_t streets_desired_phase_plan_generator::get_min_green() const {
        return config.min_green;
    }

    void streets_desired_phase_plan_generator::set_max_green(const uint64_t _max_green){
        config.max_green = _max_green;
    }

    uint64_t streets_desired_phase_plan_generator::get_max_green() const {
        return config.max_green;
    }

    void streets_desired_phase_plan_generator::set_desired_future_move_group_count(const uint8_t count){
        config.desired_future_move_group_count = count;
    }

    uint8_t streets_desired_phase_plan_generator::get_desired_future_move_group_count() const {
        return config.desired_future_move_group_count;
    }

    std::unordered_map<uint8_t, std::vector<int>> streets_desired_phase_plan_generator::get_signal_group_entry_lane_mapping() const {
        return signal_group_entry_lane_mapping;
    }

}

