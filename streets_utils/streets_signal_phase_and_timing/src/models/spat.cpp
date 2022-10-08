#include "spat.h"

namespace signal_phase_and_timing{

    std::string spat::toJson() {
        rapidjson::Document doc;
        auto allocator = doc.GetAllocator();
        // Create SPaT JSON value
        rapidjson::Value spat(rapidjson::kObjectType);
        // Read lock
        std::shared_lock lock(spat_lock);
        // Populate SPat JSON
        spat.AddMember("time_stamp", timestamp, allocator);
        spat.AddMember( "name", name, allocator);
        if ( !intersections.empty() ) {
            rapidjson::Value list(rapidjson::kArrayType);
            for (const auto &intersection : intersections ) {
                list.PushBack(intersection.toJson(allocator), allocator);
            }
            spat.AddMember("intersections", list, allocator);
        }else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersections property!");
        }
        rapidjson::StringBuffer buffer;
        try {
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            spat.Accept(writer);
        }
        catch( const std::exception &e ) {
            throw signal_phase_and_timing_exception(e.what());
        }
        return buffer.GetString();
    }

    void spat::fromJson(const std::string &json )  {
        rapidjson::Document doc;
        doc.Parse(json.c_str());
        if (doc.HasParseError()) {
            throw signal_phase_and_timing_exception("SPaT message JSON is misformatted. JSON parsing failed!");  
        }
        // Write lock
        std::unique_lock  lock(spat_lock);
        if ( doc.HasMember("time_stamp") && doc.FindMember("time_stamp")->value.IsUint64()) {
            timestamp =  (uint32_t) doc["time_stamp"].GetUint64(); // OPTIONAL in J2735 SPaT definition
        } 
        if (doc.FindMember("name")->value.IsString() ) {
            name = doc["name"].GetString();  // OPTIONAL see J2735 SPaT definition
        }
        if ( doc.FindMember("intersections")->value.IsArray() ) {
            // REQUIRED see J2735 SPaT definition
            // Clear intersection state list in case it is populated.
            intersections.clear();
            for ( const auto &intersection : doc["intersections"].GetArray() ){
                intersection_state cur_state;
                cur_state.fromJson(intersection);
                intersections.push_back(cur_state);
            }
        }
        else {
            throw signal_phase_and_timing_exception("SPaT message is missing required intersections property!");
        }
            
        
    }

    void spat::update( ntcip::ntcip_1202_ext &ntcip_data, bool use_ntcip_timestamp ){
        if ( phase_to_signal_group.empty() || intersections.front().name.empty() || intersections.front().id == 0) {
            throw signal_phase_and_timing_exception("Before updating SPAT with NTCIP information spat::initialize() must be called! See README documentation!");
        }
        // Write lock
        std::unique_lock lock(spat_lock);
        if ( use_ntcip_timestamp ) {
            set_timestamp_ntcip( ntcip_data.get_timestamp_seconds_of_day(), ntcip_data.spat_timestamp_msec);
        } 
        else {
            set_timestamp_local();
        }
        update_intersection_state( ntcip_data );
        
        
        
    }

    void spat::initialize_intersection(const std::string &intersection_name, const int intersection_id, const std::unordered_map<int,int> &_phase_number_to_signal_group ) {
        // Write lock
        std::unique_lock lock(spat_lock);
        if (!intersections.empty()) {
            intersections.clear();
        }
        intersection_state cur_state;
        cur_state.name = intersection_name;
        cur_state.id =  (uint16_t) intersection_id;
        phase_to_signal_group = _phase_number_to_signal_group;
        cur_state.initialize_movement_states( phase_to_signal_group );
        intersections.push_back(cur_state);
    }

    void spat::set_timestamp_ntcip(const uint32_t second_of_day, const uint16_t millisecond_of_second ) {
        if ( !intersections.empty()) {
            intersection_state &intersection = intersections.front();
            intersection.set_timestamp_ntcip(second_of_day, millisecond_of_second);
            timestamp = intersection.moy;
        }
        else {
            throw signal_phase_and_timing_exception("Intersection State List is empty! Cannot populate timestamp information!");
        }
    }

    void spat::set_timestamp_local() {
        if ( !intersections.empty() ) {
            intersection_state &intersection = intersections.front();
            intersection.set_timestamp_local();
            timestamp =  intersection.moy;
        }
        else {
            throw signal_phase_and_timing_exception("Intersection State List is empty! Cannot populate timestamp information!");
        }
    }

    void spat::update_intersection_state( ntcip::ntcip_1202_ext &ntcip_data ) {
        if ( !intersections.empty() ) {
            intersection_state &intersection = intersections.front();
            intersection.update_movements(ntcip_data, phase_to_signal_group);
            // Update Intersection Status
            intersection.status = ntcip_data.spat_intersection_status;
            // From V2X-Hub TODO: Investigate
            intersection.revision = 1;
        }
        else {
            throw signal_phase_and_timing_exception("Intersection State List is empty! Cannot populate status information!");
        }
    }

    void spat::update_spat_with_candidate_dpp(const streets_desired_phase_plan::streets_desired_phase_plan& candidate_dpp, const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state)
    {
        //Removing all the future movement events from each intersection state because the future movements event should be recalculated using desired phase plan
        intersections.front().clear_future_movement_events();
        auto states = intersections.front().states;
        // Loop through desired phase plan
        bool is_procssing_first_desired_green = true;
        for (const auto &desired_sg_green_timing : candidate_dpp.desired_phase_plan)
        {
            if (desired_sg_green_timing.signal_groups.empty())
            {
                throw signal_phase_and_timing_exception("Desired phase plan signal group ids list is empty. No update.");
            }

            // Loop through current spat and assuming that current spat only has the current movement event and does not contain future movement events
            for (const auto &movement_state : states)
            {
                int current_signal_group_id = movement_state.signal_group;

                // Before we add future movement events to the movement event list, the current movement event list should only contains the current movement event
                if (movement_state.state_time_speed.size() > 1)
                {
                    throw signal_phase_and_timing_exception("Movement event list has more than one events, not usable when adding future movement events. Associated with Signal Group: " + std::to_string(current_signal_group_id));
                }

                // Get movement_state by reference. With this reference, it can update the original SPAT movement state list
                auto &cur_movement_state_ref = intersections.front().get_movement(current_signal_group_id);
                // Processing the next current or first desired future movement event from the desired phase plan
                if (is_procssing_first_desired_green)
                {
                   process_first_desired_green(cur_movement_state_ref, desired_sg_green_timing, tsc_state);
                }
                else
                {
                    // Processing the next future desired future movement event from the desired phase plan
                    process_second_onward_desired_green(cur_movement_state_ref, desired_sg_green_timing, tsc_state);
                }
            }
            is_procssing_first_desired_green = false;
        }
    }

    void spat::process_first_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                        const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing,
                                        const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state) const
    {
        auto desired_green_signal_group_ids = desired_sg_green_timing.signal_groups;
        auto desired_green_start_time_epoch = desired_sg_green_timing.start_time;
        auto desired_green_end_time_epoch = desired_sg_green_timing.end_time;       

        // If SPAT Current signal group id is in the current desired phase plan signal group ids, process desired green event for the SPAT 
        // current movement event
        int current_signal_group_id = cur_movement_state_ref.signal_group;
        // Get yellow change and red clearance time interval for signal group in movement group with the largest combination
        // of yellow change and red clearance time interval.
        int signal_group_with_largest_clearance = find_max_desired_yellow_duration_red_clearance_pair(desired_sg_green_timing.signal_groups, tsc_state);
        int desired_yellow_duration = tsc_state->tsc_config_list.empty() ? 0 : tsc_state->get_signal_group_configuration_by_sg(signal_group_with_largest_clearance).yellow_change_duration;
        int desired_red_clearance = tsc_state->tsc_config_list.empty() ? 0 : tsc_state->get_signal_group_configuration_by_sg(signal_group_with_largest_clearance).red_clearance;
        SPDLOG_DEBUG("process signal group = {0} \t AND First desired sg ids = [{1} , {2}]", 
                    current_signal_group_id, desired_green_signal_group_ids.front(), 
                    desired_green_signal_group_ids.back());        
        SPDLOG_DEBUG("desired sg id={0} \tdesired_yellow_duration = {1} \t AND desired_red_clearance = {2}",signal_group_with_largest_clearance, desired_yellow_duration, desired_red_clearance);
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
                throw signal_phase_and_timing_exception("SPAT current movement event has to be green or red event state!");
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
                int spat_current_yellow_duration = tsc_state->tsc_config_list.empty()? 0 : tsc_state->get_signal_group_configuration_by_sg(current_signal_group_id).yellow_change_duration;
                SPDLOG_DEBUG("signal_group_with_largest_clearance={0} \tdesired_yellow_duration = {1} \t AND desired_red_clearance = {2}",signal_group_with_largest_clearance, desired_yellow_duration, desired_red_clearance);
       
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
                throw signal_phase_and_timing_exception("SPAT current movement event has to be red or yellow if its signal group id is not in desired signal group ids!");
            }
        }
    }

    void spat::process_second_onward_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
                                                                        const streets_desired_phase_plan::signal_group2green_phase_timing &desired_sg_green_timing,
                                                                        const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state
                                                                        ) const
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
        int signal_group_with_largest_clearance = find_max_desired_yellow_duration_red_clearance_pair(desired_sg_green_timing.signal_groups, tsc_state);
        int desired_yellow_duration = tsc_state->tsc_config_list.empty() ? 0 : tsc_state->get_signal_group_configuration_by_sg(signal_group_with_largest_clearance).yellow_change_duration;
        int desired_red_clearance = tsc_state->tsc_config_list.empty() ? 0 : tsc_state->get_signal_group_configuration_by_sg(signal_group_with_largest_clearance).red_clearance;
        SPDLOG_DEBUG("process signal group = {0} \t AND second and onwards desired sg ids = [{1} , {2}]", 
                    current_signal_group_id, 
                    desired_green_signal_group_ids.front(), 
                    desired_green_signal_group_ids.back());
        SPDLOG_DEBUG("desired sg id={0} \tdesired_yellow_duration = {1} \t AND desired_red_clearance = {2}",signal_group_with_largest_clearance, desired_yellow_duration, desired_red_clearance);
       
        if (std::count(desired_green_signal_group_ids.begin(), desired_green_signal_group_ids.end(), current_signal_group_id))
        {
            if (cur_movement_state_ref.state_time_speed.back().event_state != signal_phase_and_timing::movement_phase_state::stop_and_remain)
            {
                throw signal_phase_and_timing_exception("The last movement event has to be red to turn green again");
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
                throw signal_phase_and_timing_exception("For processing future movement events, the movement event has to be red if its signal group is not in the desired signal groups.");
            }
        }
    }

    void spat::populate_movement_event(signal_phase_and_timing::movement_event &movemnet_event_to_populate, 
                                                            const signal_phase_and_timing::movement_phase_state &phase_state, 
                                                            const uint64_t start_time_epoch, 
                                                            const uint64_t end_time_epoch)  const
    {
        movemnet_event_to_populate.event_state = phase_state;
        movemnet_event_to_populate.timing.set_start_time(start_time_epoch);
        movemnet_event_to_populate.timing.set_min_end_time(end_time_epoch);
    }

    void spat::append_full_green_yellow_red_phases_by_desired_green(signal_phase_and_timing::movement_state &cur_movement_state_ref, 
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
    
    int spat::find_max_desired_yellow_duration_red_clearance_pair(std::vector<int> desired_signal_groups, const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state) const
    {
        int return_sg_id = desired_signal_groups.front();
        int total = 0;
        for (auto sg : desired_signal_groups)
        {
            int desired_yellow_duration = tsc_state->tsc_config_list.empty()? 0 :tsc_state->get_signal_group_configuration_by_sg(sg).yellow_change_duration;
            int desired_red_clearance = tsc_state->tsc_config_list.empty()? 0 :tsc_state->get_signal_group_configuration_by_sg(sg).red_clearance;
            int local_total =  desired_yellow_duration+ desired_red_clearance;

            if (local_total > total)
            {
                total = local_total;
                return_sg_id = sg;
            }
        }
        return return_sg_id;
    }
    
    intersection_state spat::get_intersection() {
        std::shared_lock lock(spat_lock);
        if (intersections.empty())
            throw signal_phase_and_timing_exception("No intersection included currently in SPaT!"); 
        return intersections.front();

    }

    std::string spat::get_name() {
        std::shared_lock lock(spat_lock);
        return name;
    }

    uint32_t spat::get_timestamp() {
        std::shared_lock lock(spat_lock);
        return timestamp;
    }

    void spat::set_intersection(const intersection_state &intersection) {
        // Write lock
        std::unique_lock lock(spat_lock);
        intersections.clear();
        intersections.push_front(intersection);
    }

    bool spat::operator==(const spat &other) const{
        return timestamp == other.timestamp && name == other.name && intersections == other.intersections;
    }

    bool spat::operator!=(const spat &other) const{
        return !operator==(other);
    }
}