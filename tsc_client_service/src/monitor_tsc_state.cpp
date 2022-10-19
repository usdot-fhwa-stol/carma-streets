#include "monitor_tsc_state.h"

namespace traffic_signal_controller_service
{
    tsc_state::tsc_state(std::shared_ptr<snmp_client> snmp_client) : snmp_client_worker_(snmp_client)
    {
        
    }

    bool tsc_state::initialize() {
        try {
            // Map signal group ids and phase nums
            //Get phase number given a signal group id
            int max_channels_in_tsc = get_max_channels();
            std::vector<int> vehicle_phase_channels;
            std::vector<int> ped_phase_channels;
            get_phase_channels(max_channels_in_tsc, vehicle_phase_channels, ped_phase_channels);
            map_phase_and_signalgroup(vehicle_phase_channels, true);
            map_phase_and_signalgroup(ped_phase_channels, false);
            // Get phase sequences for ring 1 and ring 2
            phase_seq_ring1_ = phase_seq(1);
            phase_seq_ring2_ = phase_seq(2);

            // Define state of each signal group
            for (const auto& signal_group : signal_group_phase_map_)
            {
                int phase_num = signal_group.second;
                signal_group_state state;
                state.phase_num = phase_num;
                state.max_green = get_max_green(phase_num);
                state.min_green = get_min_green(phase_num);
                state.yellow_duration = get_yellow_duration(phase_num);
                // Define green duration as min/max as decided
                state.green_duration = state.min_green;
                state.red_clearance = get_red_clearance(phase_num);
                state.phase_seq = get_following_phases(phase_num);
                state.concurrent_signal_groups = get_concurrent_signal_groups(phase_num);
                signal_group_state_map_.insert(std::make_pair(signal_group.first, state));
            }

        
            // Loop through states once other state parameters are defined to get the red duration
            for(auto& [signalgroup_id, state] : signal_group_state_map_)
            {
                
                state.red_duration = get_red_duration(state.phase_num);
            }

            // Define tsc config state
            define_tsc_config_state();
            
            // Print signal_group map
            for (const auto& [signalgroup_id, phase] : signal_group_phase_map_)
            {
                SPDLOG_DEBUG("Signal group id: {0} phase: {1}", signalgroup_id, phase);
            }
            // Print state map
            for (const auto& [signalgroup_id, state] : signal_group_state_map_)
            {
                SPDLOG_DEBUG("Signal group id: {0}", signalgroup_id);
                SPDLOG_DEBUG("Phase num: {0}", state.phase_num);
                SPDLOG_DEBUG("Max green: {0}", state.max_green);
                SPDLOG_DEBUG("Min green: {0}", state.min_green);
                SPDLOG_DEBUG("Green Duration: {0}", state.green_duration);
                SPDLOG_DEBUG("yellow duration: {0}", state.yellow_duration);
                SPDLOG_DEBUG("Red clearance: {0}", state.red_clearance);
                SPDLOG_DEBUG("Red duration: {0}", state.red_duration);

                SPDLOG_DEBUG("Phase sequence:");
                for(auto phase : state.phase_seq){
                    SPDLOG_DEBUG("{0}", phase);
                }

                SPDLOG_DEBUG("Concurrent Phases:");
                for(auto signal_group : state.concurrent_signal_groups){
                    SPDLOG_DEBUG("{0}", signal_group);
                }  
            }
            return true;
        }catch ( const snmp_client_exception &e ) {
            SPDLOG_ERROR("Exception encounters during initialization: \n {0}",e.what());
            return false;
        }
    }

    void tsc_state::define_tsc_config_state()
    {   
        streets_tsc_configuration::tsc_configuration_state tsc_config;
        for (const auto& [signal_group, state] : signal_group_state_map_)
        {
            streets_tsc_configuration::signal_group_configuration signal_group_config;
            signal_group_config.signal_group_id = static_cast<uint8_t>(signal_group);
            signal_group_config.yellow_change_duration = static_cast<uint16_t>(state.yellow_duration);
            signal_group_config.red_clearance = static_cast<uint16_t>(state.red_clearance);
            for(auto concurrent_signal_group : state.concurrent_signal_groups){
                signal_group_config.concurrent_signal_groups.push_back(static_cast<uint8_t>(concurrent_signal_group));
            }
            tsc_config.tsc_config_list.push_back(signal_group_config);
        }
        tsc_config_state_ptr_ = std::make_shared<streets_tsc_configuration::tsc_configuration_state>(tsc_config);
    }

    const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state>& tsc_state::get_tsc_config_state() const
    {
        return tsc_config_state_ptr_;
    }

    signal_phase_and_timing::movement_event tsc_state::get_following_event(const signal_phase_and_timing::movement_event& current_event,
                                                                 uint64_t current_event_end_time, const signal_group_state& phase_state) const
    {
        signal_phase_and_timing::movement_event next_event;
        switch (current_event.event_state){
            case signal_phase_and_timing::movement_phase_state::protected_movement_allowed: //Green
                // Create next movement - yellow
                next_event.event_state = signal_phase_and_timing::movement_phase_state::protected_clearance;
                next_event.timing.set_start_time(current_event_end_time);
                next_event.timing.set_min_end_time(current_event_end_time + phase_state.yellow_duration);
                next_event.timing.set_max_end_time(current_event_end_time + phase_state.yellow_duration);
                break;

            case signal_phase_and_timing::movement_phase_state::protected_clearance: //Yellow
                // Create next movement - red
                next_event.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain;
                next_event.timing.set_start_time(current_event_end_time); 
                next_event.timing.set_min_end_time(current_event_end_time + phase_state.red_duration);
                next_event.timing.set_max_end_time(current_event_end_time + phase_state.red_duration);
                break;

            case signal_phase_and_timing::movement_phase_state::stop_and_remain:  //Red
                // Create next movement - green
                next_event.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed;
                next_event.timing.set_start_time(current_event_end_time);
                next_event.timing.set_min_end_time(current_event_end_time + phase_state.green_duration);
                next_event.timing.set_max_end_time(current_event_end_time + phase_state.green_duration);
                break;

            default:
                SPDLOG_ERROR("This movement phase is not supported. Movement phase type: {0}", int(current_event.event_state));
                throw snmp_client_exception("Failed request for unsupported movement phase type: " + std::to_string(int(current_event.event_state)));
        }
        return next_event;
    }

    void tsc_state::add_future_movement_events(std::shared_ptr<signal_phase_and_timing::spat> spat_ptr)
    {
        // Modify spat according to phase configuration
        // Note: Only first intersection is populated
        auto intersection_state = spat_ptr->get_intersection();
        for (auto movement : intersection_state.states)
        {
            int signal_group_id = movement.signal_group;

            // Assumption here is that the movement states have only one event, since this method adds future events to the list.
            //Throw exception is list size is great than 1
            if (movement.state_time_speed.size() > 1)
            {
                SPDLOG_ERROR("Event list has more than one events, not usable when adding future movement events. Associated with Signal Group: {0}", signal_group_id);
                throw monitor_states_exception("Event list has more than one events, not usable when adding future movement events. Associated with Signal Group:" + std::to_string(signal_group_id));
            }

            // Get movement_state by reference
            auto& current_movement = intersection_state.get_movement(signal_group_id);

            // Get start time as epoch time
            uint64_t start_time = movement.state_time_speed.front().timing.get_epoch_start_time();
            
            // Check if signal_group_id is associated with a vehicle phase : Only vehicle phases mapped to signal_group_states
            signal_group_state phase_state;
            if(signal_group_state_map_.find(signal_group_id) == signal_group_state_map_.end()){
                continue;
            }
            
            phase_state = signal_group_state_map_[signal_group_id];

            uint64_t current_event_end_time_epoch = 0;
            signal_phase_and_timing::movement_event current_event = current_movement.state_time_speed.front();

            switch(current_event.event_state){
                case signal_phase_and_timing::movement_phase_state::stop_and_remain : //Red
                    current_event_end_time_epoch = start_time + phase_state.red_duration;
                    break;

                case signal_phase_and_timing::movement_phase_state::protected_movement_allowed : //Green
                    current_event_end_time_epoch = start_time + phase_state.green_duration;
                    break;                                                                

                case signal_phase_and_timing::movement_phase_state::protected_clearance : //Yellow
                    current_event_end_time_epoch = start_time + phase_state.yellow_duration;
                    break;

                default:
                    SPDLOG_DEBUG("This movement phase is not supported. Movement phase type: {0}", int(current_movement.state_time_speed.front().event_state));
                    throw monitor_states_exception("This movement phase is not supported. Movement phase type: " + std::to_string(int(current_movement.state_time_speed.front().event_state)));
            }
            // Update end_time for current_event
            current_movement.state_time_speed.front().timing.set_min_end_time(current_event_end_time_epoch);
            current_movement.state_time_speed.front().timing.set_max_end_time(current_event_end_time_epoch);

            for(int i = 0; i < required_following_movements_; ++i)
            {
                signal_phase_and_timing::movement_event next_event = get_following_event(current_event, current_event_end_time_epoch, phase_state);
                // Set next event to current event
                current_event = next_event;
                current_event_end_time_epoch = current_event.timing.get_epoch_min_end_time();
                //Add events to list
                current_movement.state_time_speed.push_back(next_event);
            }
            
        }
        spat_ptr->set_intersection(intersection_state);
        
    }

    std::vector<int> tsc_state::get_following_phases(int phase_num)
    {
        std::vector<int> sequence;

        // Add phase sequence to signal group state
        for(size_t i = 0; i < phase_seq_ring1_.size();++i)
        {
            if(phase_seq_ring1_[i] == phase_num)
            {
                sequence.insert(sequence.begin(), phase_seq_ring1_.begin() + i, phase_seq_ring1_.end());
                sequence.insert(sequence.end(), phase_seq_ring1_.begin(), phase_seq_ring1_.begin() + i);

                return sequence; 
            }
        }

        for(size_t i = 0; i < phase_seq_ring2_.size();++i)
        {
            if(phase_seq_ring2_[i] == phase_num)
            {
                sequence.insert(sequence.begin(), phase_seq_ring2_.begin() + i, phase_seq_ring2_.end());
                sequence.insert(sequence.end(), phase_seq_ring2_.begin(), phase_seq_ring2_.begin() + i);

                return sequence; 
            }
        }
        // No following phase information found
        throw snmp_client_exception("No following phases found");
    }

    int tsc_state::get_max_channels() const {
        request_type request_type = request_type::GET;
        snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.type = snmp_response_obj::response_type::INTEGER;

        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type, max_channels_in_tsc))
        {
            throw snmp_client_exception("Failed to get max channels!");
        }

        return (int) max_channels_in_tsc.val_int;
    }

    void tsc_state::get_phase_channels(int max_channels, std::vector<int>& vehicle_phase_channels, std::vector<int>& ped_phase_channels) const{
        
        // Loop through channel control types and add channels with vehicle phase to list
        snmp_response_obj control_type;
        control_type.type = snmp_response_obj::response_type::INTEGER;
        
        request_type request_type = request_type::GET;
        for(int channel_num = 1; channel_num <= max_channels; ++channel_num)
        {
            std::string control_type_parameter_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(channel_num);

            if(!snmp_client_worker_->process_snmp_request(control_type_parameter_oid, request_type, control_type))
            {
                throw snmp_client_exception("Failed to get channel control type");
            }

            if(control_type.val_int == 2)
            {
                vehicle_phase_channels.push_back(channel_num);
            }
            else if(control_type.val_int == 3)
            {
                ped_phase_channels.push_back(channel_num);
            }

        }

        if(vehicle_phase_channels.empty()){
            SPDLOG_WARN("Found no active vehicle phases");
        }
        if(ped_phase_channels.empty()){
            SPDLOG_DEBUG("Found no active ped phases");
        }
        
        SPDLOG_DEBUG("Number of vehicle phase channels found: {0}", vehicle_phase_channels.size());
        SPDLOG_DEBUG("Number of ped phase channels found: {0}", ped_phase_channels.size());
        
    }

    void tsc_state::map_phase_and_signalgroup(const std::vector<int>& phase_channels, bool is_source_vehicle_channel)
    {
        // According to NTCIP 1202 v03 documentation Signal Group ID in a SPAT message is the Channel Number from TSC

        // Get phases associated with vehicle phase channels
        request_type request_type = request_type::GET;
        for(int channel : phase_channels)
        {
            snmp_response_obj phase_num;
            phase_num.type = snmp_response_obj::response_type::INTEGER;

            std::string control_source_parameter_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(channel);
            snmp_client_worker_->process_snmp_request(control_source_parameter_oid, request_type, phase_num);

            // According to NTCIP 1202 v03 returned value of 0 here would mean a phase is not associated with the channel
            if(phase_num.val_int != 0)
            {
                if(is_source_vehicle_channel){
                    vehicle_phase_num_map_.insert(std::make_pair(phase_num.val_int, channel));
                    signal_group_phase_map_.insert(std::make_pair(channel, phase_num.val_int));
                }
                else{
                    ped_phase_num_map_.insert(std::make_pair(phase_num.val_int, channel));
                }
                SPDLOG_DEBUG("Found mapping between signal group: {0} and phase num: {1}", channel , phase_num.val_int );
            }
        }
        
    }

    int tsc_state::get_phase_number(const int signal_group_id ) {
        if (signal_group_id >= 1) {
            if ( signal_group_phase_map_.find(signal_group_id) != signal_group_phase_map_.end() ) {
                return signal_group_phase_map_[signal_group_id];
            } 
            else {
                throw monitor_states_exception("No phase number found for signal id " + std::to_string(signal_group_id) + "!");
            }
        }
        else {
            throw monitor_states_exception("Signal group ids less than 1 are invalid!");
        }
    }

    int tsc_state::get_signal_group_id(const int phase_number ) {
        if (phase_number >= 1) {
            if ( vehicle_phase_num_map_.find(phase_number) != vehicle_phase_num_map_.end() ) {
                return vehicle_phase_num_map_[phase_number];
            } 
            else {
                throw monitor_states_exception("No signal group id found for phase number " + std::to_string(phase_number) + "!");
            }
        }
        else {
            throw monitor_states_exception("Phase numbers less than 1 are invalid!");
        }
    }

    int tsc_state::get_min_green(int phase_num) const
    {
        request_type request_type = request_type::GET;
        std::string min_green_parameter_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(phase_num);

        snmp_response_obj min_green;
        min_green.type = snmp_response_obj::response_type::INTEGER;

        snmp_client_worker_->process_snmp_request(min_green_parameter_oid, request_type, min_green);

        return (int) min_green.val_int * 1000; //Convert seconds to milliseconds
    }

    int tsc_state::get_max_green(int phase_num) const
    {
        request_type request_type = request_type::GET;

        std::string max_green_parameter_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(phase_num);
        snmp_response_obj max_green;
        max_green.type = snmp_response_obj::response_type::INTEGER;
        
        snmp_client_worker_->process_snmp_request(max_green_parameter_oid, request_type, max_green);

        return (int) max_green.val_int * 1000; //Convert seconds to milliseconds
    }

    int tsc_state::get_yellow_duration(int phase_num) const
    {
        request_type request_type = request_type::GET;
        std::string yellow_duration_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(phase_num);

        snmp_response_obj yellow_duration;
        yellow_duration.type = snmp_response_obj::response_type::INTEGER;
        
        snmp_client_worker_ ->process_snmp_request(yellow_duration_oid, request_type, yellow_duration);

        return (int) yellow_duration.val_int * 100; //Convert to milliseconds. NTCIP returned value is in tenths of seconds
    }

    int tsc_state::get_red_clearance(int phase_num) const
    {
        request_type get_request = request_type::GET;
        std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(phase_num);

        snmp_response_obj red_clearance;
        red_clearance.type = snmp_response_obj::response_type::INTEGER;

        snmp_client_worker_->process_snmp_request(red_clearance_oid, get_request, red_clearance);

        return (int) red_clearance.val_int * 100; //Convert to milliseconds. NTCIP returned value is in tenths of seconds
    }

    int tsc_state::get_red_duration(int phase_num)
    {
        //red duration is calculated as red_clearance time for current phase + (green duration + yellow duration + red_clearance) for other phases in sequence

        // Find signal state associated with phase num
        int current_signal_group;
        current_signal_group = get_signal_group_id(phase_num); 
        auto current_signal_group_state = signal_group_state_map_[current_signal_group];
        // Only add clearance time for current phase
        int red_duration = current_signal_group_state.red_clearance;
        
        for(auto phase : current_signal_group_state.phase_seq)
        {
            // Find state params for each phase in seq
            int seq_signal_group;
            seq_signal_group = get_signal_group_id(phase);
            auto seq_signal_group_state = signal_group_state_map_[seq_signal_group];
            if(phase == phase_num)
            {
                continue;
            }
            else{
                red_duration += seq_signal_group_state.green_duration + seq_signal_group_state.yellow_duration + seq_signal_group_state.red_clearance;
            }
        }
        
        return red_duration; 
    }

    std::vector<int> tsc_state::phase_seq(int ring_num)
    {
        std::vector<int> phase_seq;
        // Read sequence 1 data for given ring
        request_type request_type = request_type::GET;
        std::string phase_seq_oid= ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(ring_num); 
        snmp_response_obj seq_data;
        seq_data.type = snmp_response_obj::response_type::STRING;
        snmp_client_worker_->process_snmp_request(phase_seq_oid, request_type, seq_data);
        
        //extract phase numbers from strings
        for(auto seq_val : seq_data.val_string)
        {   
            auto phase = int(seq_val);
            if(vehicle_phase_num_map_.find(phase)!= vehicle_phase_num_map_.end() && phase != 0 ){
                phase_seq.push_back(phase);
            }
            
        }

        if(phase_seq.empty())
        {
            SPDLOG_WARN("No phases found in sequence 1 for ring {0}", ring_num);
        }

        return phase_seq;

    }

    std::vector<int> tsc_state::get_concurrent_signal_groups(int phase_num)
    {

        std::vector<int> concurrent_signal_groups;
        std::string concurrent_phases_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(phase_num);

        snmp_response_obj concurrent_phase_data;
        request_type request_type = request_type::GET;
        concurrent_phase_data.type = snmp_response_obj::response_type::STRING;
        snmp_client_worker_->process_snmp_request(concurrent_phases_oid, request_type, concurrent_phase_data);

        //extract phase numbers from strings
        for(auto con_phase :  concurrent_phase_data.val_string)
        {   
            concurrent_signal_groups.push_back(get_signal_group_id(int(con_phase)));
            
        }

        if(concurrent_signal_groups.empty())
        {
            SPDLOG_WARN("No concurrent signal groups found for phase {0}", phase_num);
        }

        return concurrent_signal_groups;
    }

    const std::unordered_map<int,int>& tsc_state::get_ped_phase_map()
    {
        return ped_phase_num_map_;
    }

    const std::unordered_map<int,int>& tsc_state::get_vehicle_phase_map()
    {
        return vehicle_phase_num_map_;
    }

    std::unordered_map<int, signal_group_state>& tsc_state::get_signal_group_state_map()
    {
        return signal_group_state_map_;
    }        

}