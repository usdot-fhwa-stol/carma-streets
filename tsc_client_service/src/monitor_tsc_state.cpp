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
            // Loop through all channels and check which are associated with a vehicle phase and pedestrian phase
            std::vector<int> vehicle_channels; 
            std::vector<int> ped_channels;
            // Get channel source for each channel in TSC and allocate to vehicle/ped depending on source type
            get_channels(max_channels_in_tsc, vehicle_channels, ped_channels);
            
            // Get Phases associated with vehicle channels and pedestrian channels
            vehicle_phase_2signalgroup_map_ = get_phases_associated_with_channel(vehicle_channels);
            ped_phase_2signalgroup_map_ = get_phases_associated_with_channel(ped_channels);
            
            int max_rings_in_tsc = get_max_rings();
            // Get vehicle sequence for active phases. By default in sequence 1
            std::vector<std::vector<int>> active_ring_sequences = get_active_ring_sequences(max_rings_in_tsc, vehicle_phase_2signalgroup_map_);
            // Update vehicle phase map to only include phases in active ring sequence. Also construct signal group to vehicle phase map
            map_phase_and_signalgroup(active_ring_sequences, vehicle_phase_2signalgroup_map_, signal_group_2vehiclephase_map_);

            // Define state for each signal group
            for (const auto& [signal_group, vehicle_phase] : signal_group_2vehiclephase_map_)
            {
                int phase_num = vehicle_phase;
                signal_group_state state;
                state.phase_num = phase_num;
                state.max_green = get_max_green(phase_num);
                state.min_green = get_min_green(phase_num);
                state.yellow_duration = get_yellow_duration(phase_num);
                // Define green duration as min/max as decided in configuration
                state.green_duration = state.min_green;
                state.red_clearance = get_red_clearance(phase_num);
                state.phase_seq = get_following_phases(phase_num, active_ring_sequences);
                state.concurrent_signal_groups = get_concurrent_signal_groups(phase_num);
                auto pair = std::make_pair(signal_group, state);
                signal_group_2tsc_state_map_.insert(pair);
            }

            // Loop through states once other state parameters are defined to get the red duration
            for(auto& [signalgroup_id, state] : signal_group_2tsc_state_map_)
            {
                
                state.red_duration = get_red_duration(state.phase_num);
            }

            // Define tsc config state - Needed to construct message sent to SO service 
            tsc_config_state_ptr_ = define_tsc_config_state();

            // Print signal_group map
            for (const auto& [signalgroup_id, phase] : signal_group_2vehiclephase_map_)
            {
                SPDLOG_DEBUG("Signal group id: {0} phase: {1}", signalgroup_id, phase);
            }
            // Print state map
            for (const auto& [signalgroup_id, state] : signal_group_2tsc_state_map_)
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

                SPDLOG_DEBUG("Concurrent Signal Groups:");
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

    std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_state::define_tsc_config_state()
    {   
        streets_tsc_configuration::tsc_configuration_state tsc_config;
        for (const auto& [signal_group, state] : signal_group_2tsc_state_map_)
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
        return std::make_shared<streets_tsc_configuration::tsc_configuration_state>(tsc_config);
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

    std::string tsc_state::vector_to_string(const std::vector<int> &v) const {
        std::string v_string = "[";
        for (auto element: v ){
            v_string.append(std::to_string(element));
            if ( element != v.back()) {
                v_string.append(", ");
            }
        }
        v_string.append("]");
        return v_string;
}

    void tsc_state::poll_vehicle_pedestrian_calls() {
        // Make SNMP requests to get vehicle and pedestrian calls 
        // for phases 1-16 (.1 gets 1-8, .2 gets 9-16)
        auto request_type = streets_snmp_cmd::REQUEST_TYPE::GET;
        std::string vehicle_call_phases_1_8 = ntcip_oids::PHASE_STATUS_GROUP_VEH_CALLS + ".1";
        std::string vehicle_call_phases_9_16 = ntcip_oids::PHASE_STATUS_GROUP_VEH_CALLS + ".2";
        std::string pedestrian_call_phases_1_8 = ntcip_oids::PHASE_STATUS_GROUP_PED_CALLS + ".1";
        std::string pedestrian_call_phases_9_16 = ntcip_oids::PHASE_STATUS_GROUP_PED_CALLS + ".2";

        streets_snmp_cmd::snmp_response_obj veh_call_1_8;
        veh_call_1_8.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        streets_snmp_cmd::snmp_response_obj veh_call_9_16;
        veh_call_9_16.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        streets_snmp_cmd::snmp_response_obj ped_call_1_8;
        ped_call_1_8.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        streets_snmp_cmd::snmp_response_obj ped_call_9_16;
        ped_call_9_16.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        snmp_client_worker_ ->process_snmp_request(vehicle_call_phases_1_8, request_type, veh_call_1_8);
        snmp_client_worker_ ->process_snmp_request(vehicle_call_phases_9_16, request_type, veh_call_9_16);
        snmp_client_worker_ ->process_snmp_request(pedestrian_call_phases_1_8, request_type, ped_call_1_8);
        snmp_client_worker_ ->process_snmp_request(pedestrian_call_phases_9_16, request_type, ped_call_9_16);

        SPDLOG_INFO("Response Veh {0}, {1} Response Ped {2}, {3}", veh_call_1_8.val_int, veh_call_9_16.val_int, ped_call_1_8.val_int, ped_call_9_16.val_int);

        // Process returned int bitwise to get vehicle/pedestrian call information for 8 phases and convert to signal groups
        auto veh_resp_1_8 = convert_veh_phases_to_signal_groups( process_bitwise_response(veh_call_1_8,0));
        auto veh_resp_9_16 = convert_veh_phases_to_signal_groups(process_bitwise_response(veh_call_9_16,8));
        auto ped_resp_1_8 = convert_ped_phases_to_signal_groups(process_bitwise_response(ped_call_1_8,0));
        auto ped_resp_9_16 = convert_ped_phases_to_signal_groups(process_bitwise_response(ped_call_9_16,8));
        
        // Clear previously saved vehicle/pedestrian information and replace with new information

        vehicle_calls.clear();
        vehicle_calls.insert(vehicle_calls.end(), veh_resp_1_8.begin(), veh_resp_1_8.end());
        vehicle_calls.insert(vehicle_calls.end(), veh_resp_9_16.begin(), veh_resp_9_16.end());
        
        pedestrian_calls.clear();
        pedestrian_calls.insert(pedestrian_calls.end(), ped_resp_1_8.begin(), ped_resp_1_8.end());
        pedestrian_calls.insert(pedestrian_calls.end(), ped_resp_9_16.begin(), ped_resp_9_16.end());

        SPDLOG_INFO("Pedestrian calls {0}, Vehicle calls, {1}", vector_to_string(pedestrian_calls), vector_to_string(vehicle_calls));
    }

    std::vector<int> tsc_state::process_bitwise_response( const streets_snmp_cmd::snmp_response_obj &response, int offset ) const{
        /**
         * Response value is 8 bit int in which each bit is interpreted individually as 1 or 0. 1 
         * indicates that the vehicle phase for that bit is committed to be next. 0 indicates this 
         * phase is not committed to be next.
         * 
         * bit 0 represent vehicle phase 1
         * bit 1 represent vehicle phase 2
         * bit 2 represent vehicle phase 3
         * bit 3 represent vehicle phase 4
         * bit 4 represent vehicle phase 5
         * bit 5 represent vehicle phase 6
         * bit 6 represent vehicle phase 7
         * bit 7 represent vehicle phase 8
         * 
         */
        std::vector<int> phase_numbers;
        for (uint i = 0; i < 8; ++i) {
            if ((response.val_int >> i) & 1) {
                // Add any signal group for phase that has bit as 1
                auto phase_number = i+1+offset;
                SPDLOG_INFO("Adding phase {0}", phase_number);
                phase_numbers.push_back(phase_number);
            }
        }
        return phase_numbers;
    }

    std::vector<int> tsc_state::get_pedestrian_calls() const {
        return pedestrian_calls;
    }

    std::vector<int> tsc_state::get_vehicle_calls() const {
        return vehicle_calls;
    }
    
    std::vector<int> tsc_state::convert_ped_phases_to_signal_groups( const std::vector<int> &ped_phases ) const{
        std::vector<int> signal_groups;
        for (auto ped_phase : ped_phases) {
            signal_groups.push_back(get_pedestrian_signal_group_id(ped_phase));
        }
        return signal_groups;
    }

    std::vector<int> tsc_state::convert_veh_phases_to_signal_groups( const std::vector<int> &veh_phases )  const{
        std::vector<int> signal_groups;
        for (auto veh_phase : veh_phases) {
            signal_groups.push_back(get_vehicle_signal_group_id(veh_phase));
        }
        return signal_groups;
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
            // Throw exception is list size is great than 1
            if (movement.state_time_speed.size() > 1)
            {
                throw monitor_states_exception("Event list has more than one events, not usable when adding future movement events. Associated with Signal Group:" + std::to_string(signal_group_id));
            }

            // Get movement_state by reference
            auto& current_movement = intersection_state.get_movement(signal_group_id);

            // Get start time as epoch time
            uint64_t start_time = movement.state_time_speed.front().timing.get_epoch_start_time();
            
            // Check if signal_group_id is associated with a vehicle phase : Only vehicle phases mapped to signal_group_states
            signal_group_state phase_state;
            if(signal_group_2tsc_state_map_.find(signal_group_id) == signal_group_2tsc_state_map_.end()){
                continue;
            }
            
            phase_state = signal_group_2tsc_state_map_[signal_group_id];

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

    std::vector<int> tsc_state::get_following_phases(int phase_num, const std::vector<std::vector<int>>& active_ring_sequences) const
    {
        std::vector<int> sequence;

        for(auto ring : active_ring_sequences)
        {
            for (uint i = 0; i < ring.size(); ++i)
            {
                if (ring[i] == phase_num)
                {
                    sequence.insert(sequence.begin(), ring.begin() + i, ring.end());
                    sequence.insert(sequence.end(), ring.begin(), ring.begin() + i);

                    return sequence;
                }
            }
        }

        // No following phase information found
        throw snmp_client_exception("No following phases found");
        
    }

    void tsc_state::map_phase_and_signalgroup(const std::vector<std::vector<int>>& active_ring_sequences, 
                    std::unordered_map<int,int>& vehicle_phase_2signalgroup_map, std::unordered_map<int, int>& signal_group_2vehiclephase_map) const
    {
        // Loop through all ring sequences and remove vehicle phase entries in map
        for(auto it = vehicle_phase_2signalgroup_map.begin(); it != vehicle_phase_2signalgroup_map.end();){
            auto phase = it->first;
            bool found_phase = false;
            for(auto ring : active_ring_sequences)
            {
                if(std::find(ring.begin(), ring.end(), phase) !=ring.end()){
                    found_phase = true;
                    break;
                }    
            }
            
            if(!found_phase)
            {
                // if phase not found in active sequence, remove entry
                it = vehicle_phase_2signalgroup_map.erase(it);
            }
            else{
                // Add entry to signal to phase map
                signal_group_2vehiclephase_map.insert(std::make_pair(it->second, it->first));
                it++;
            }
        }
        
    }

    std::unordered_map<int,int> tsc_state::get_phases_associated_with_channel(const std::vector<int>& signal_group_ids) const {
        
        std::unordered_map<int,int> phases_to_signal_group;
        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;

        for(int signal_group : signal_group_ids)
        {
            streets_snmp_cmd::snmp_response_obj phase_num;
            phase_num.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
            // Control source returns the phase associated with the signal group. Channel and signal group id is synonymous according to the NTCIP documentation
            std::string control_source_parameter_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(signal_group);
            snmp_client_worker_->process_snmp_request(control_source_parameter_oid, request_type, phase_num);

            // According to NTCIP 1202 v03 returned value of 0 here would mean a phase is not associated with the signal_group
            if(phase_num.val_int != 0)
            {
                phases_to_signal_group.insert(std::make_pair(phase_num.val_int, signal_group));
            }
            else{
                throw monitor_states_exception("No phase found associated with signal_group {0}" + std::to_string(signal_group));
            }
            SPDLOG_TRACE("Found mapping between signal group: {0} and phase num: {1}", signal_group , phase_num.val_int );

        }

        return phases_to_signal_group;

    }

    void tsc_state::get_channels(int max_channels, std::vector<int>& vehicle_channels, std::vector<int>& ped_channels) const{
        
        // Loop through channel control types and add channels with vehicle phase to list
        streets_snmp_cmd::snmp_response_obj control_type;
        control_type.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        
        auto request_type = streets_snmp_cmd::REQUEST_TYPE::GET;
        for(int channel_num = 1; channel_num <= max_channels; ++channel_num)
        {
            std::string control_type_parameter_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(channel_num);

            if(!snmp_client_worker_->process_snmp_request(control_type_parameter_oid, request_type, control_type))
            {
                throw snmp_client_exception("Failed to get channel control type");
            }

            if(control_type.val_int == 2) // If 2, phase is a vehicle phase
            {
                vehicle_channels.push_back(channel_num);
            }
            else if(control_type.val_int == 3) //If 3, phase is a pedestrian phase
            {
                ped_channels.push_back(channel_num);
            }

        }

        if(vehicle_channels.empty()){
            SPDLOG_WARN("Found no vehicle phases");
        }
        if(ped_channels.empty()){
            SPDLOG_DEBUG("Found no ped phases");
        }
        
        SPDLOG_DEBUG("Number of vehicle phase channels found: {0}", vehicle_channels.size());
        SPDLOG_DEBUG("Number of ped phase channels found: {0}", ped_channels.size());
        
    }

    

    std::vector<std::vector<int>> tsc_state::get_active_ring_sequences(int max_rings, std::unordered_map<int,int>& vehicle_phase_2signalgroup_map, int sequence) const{
        
        std::vector<std::vector<int>> active_ring_sequences;
        // Read sequence data for rings
        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;

        for(int i = 1 ; i <= max_rings; ++i){
            int ring_num = i;
            std::vector<int> phase_seq;

            std::string phase_seq_oid= ntcip_oids::SEQUENCE_DATA + "." + std::to_string(sequence) + "." + std::to_string(ring_num);
            
            streets_snmp_cmd::snmp_response_obj seq_data;
            seq_data.type = streets_snmp_cmd::RESPONSE_TYPE::STRING;
            snmp_client_worker_->process_snmp_request(phase_seq_oid, request_type, seq_data);

            //extract phase numbers from strings
            for(auto seq_val : seq_data.val_string)
            {   
                auto phase = int(seq_val);
                if(vehicle_phase_2signalgroup_map.find(phase)!= vehicle_phase_2signalgroup_map.end() && phase != 0 ){
                    phase_seq.push_back(phase);
                }
                
            }
            if (!phase_seq.empty())
            {
                active_ring_sequences.push_back(phase_seq);
            }
            else{
                SPDLOG_DEBUG("No phases found in sequence {0} for ring {1}", sequence, ring_num);
            }
        }

        return active_ring_sequences;
    }

    int tsc_state::get_max_rings() const {
        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;
        streets_snmp_cmd::snmp_response_obj max_rings_in_tsc;
        max_rings_in_tsc.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::MAX_RINGS, request_type, max_rings_in_tsc))
        {
            throw snmp_client_exception("Failed to get max rings!");
        }

        return (int) max_rings_in_tsc.val_int;
    }

    int tsc_state::get_max_channels() const {
        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;
        streets_snmp_cmd::snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type, max_channels_in_tsc))
        {
            throw snmp_client_exception("Failed to get max channels!");
        }

        return (int) max_channels_in_tsc.val_int;
    }

    int tsc_state::get_phase_number(const int signal_group_id ) {
        if (signal_group_id >= 1) {
            if ( signal_group_2vehiclephase_map_.find(signal_group_id) != signal_group_2vehiclephase_map_.end() ) {
                return signal_group_2vehiclephase_map_[signal_group_id];
            } 
            else {
                throw monitor_states_exception("No phase number found for signal id " + std::to_string(signal_group_id) + "!");
            }
        }
        else {
            throw monitor_states_exception("Signal group ids less than 1 are invalid!");
        }
    }

    int tsc_state::get_vehicle_signal_group_id(const int phase_number ) const {
        if (phase_number >= 1) {
            if ( vehicle_phase_2signalgroup_map_.find(phase_number) != vehicle_phase_2signalgroup_map_.end() ) {
                return vehicle_phase_2signalgroup_map_.at(phase_number);
            } 
            else {
                throw monitor_states_exception("No signal group id found for vehicle phase number " + std::to_string(phase_number) + "!");
            }
        }
        else {
            throw monitor_states_exception("Phase numbers less than 1 are invalid!");
        }
    }

    int tsc_state::get_pedestrian_signal_group_id(const int phase_number) const{
        if (phase_number >= 1) {
            if ( ped_phase_2signalgroup_map_.find(phase_number) != ped_phase_2signalgroup_map_.end() ) {
                return ped_phase_2signalgroup_map_.at(phase_number);
            } 
            else {
                throw monitor_states_exception("No signal group id found for pedestrian phase number " + std::to_string(phase_number) + "!");
            }
        }
        else {
            throw monitor_states_exception("Phase numbers less than 1 are invalid!");
        }
    }

    int tsc_state::get_min_green(int phase_num) const
    {
        auto request_type = streets_snmp_cmd::REQUEST_TYPE::GET;
        std::string min_green_parameter_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(phase_num);

        streets_snmp_cmd::snmp_response_obj min_green;
        min_green.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        snmp_client_worker_->process_snmp_request(min_green_parameter_oid, request_type, min_green);

        return (int) min_green.val_int * 1000; //Convert seconds to milliseconds
    }

    int tsc_state::get_max_green(int phase_num) const
    {
        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;

        std::string max_green_parameter_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(phase_num);
        streets_snmp_cmd::snmp_response_obj max_green;
        max_green.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        
        snmp_client_worker_->process_snmp_request(max_green_parameter_oid, request_type, max_green);

        return (int) max_green.val_int * 1000; //Convert seconds to milliseconds
    }

    int tsc_state::get_yellow_duration(int phase_num) const
    {
        auto request_type = streets_snmp_cmd::REQUEST_TYPE::GET;
        std::string yellow_duration_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(phase_num);

        streets_snmp_cmd::snmp_response_obj yellow_duration;
        yellow_duration.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        
        snmp_client_worker_ ->process_snmp_request(yellow_duration_oid, request_type, yellow_duration);

        return (int) yellow_duration.val_int * 100; //Convert to milliseconds. NTCIP returned value is in tenths of seconds
    }

    int tsc_state::get_red_clearance(int phase_num) const
    {
        auto get_request = streets_snmp_cmd::REQUEST_TYPE::GET;
        std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(phase_num);

        streets_snmp_cmd::snmp_response_obj red_clearance;
        red_clearance.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        snmp_client_worker_->process_snmp_request(red_clearance_oid, get_request, red_clearance);

        return (int) red_clearance.val_int * 100; //Convert to milliseconds. NTCIP returned value is in tenths of seconds
    }

    int tsc_state::get_red_duration(int phase_num)
    {
        //red duration is calculated as red_clearance time for current phase + (green duration + yellow duration + red_clearance) for other phases in sequence

        // Find signal state associated with phase num
        int current_signal_group;
        current_signal_group = get_vehicle_signal_group_id(phase_num); 
        auto current_signal_group_state = signal_group_2tsc_state_map_[current_signal_group];
        // Only add clearance time for current phase
        int red_duration = current_signal_group_state.red_clearance;
        
        for(auto phase : current_signal_group_state.phase_seq)
        {
            // Find state params for each phase in seq
            int seq_signal_group;
            seq_signal_group = get_vehicle_signal_group_id(phase);
            auto seq_signal_group_state = signal_group_2tsc_state_map_[seq_signal_group];
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

    std::vector<int> tsc_state::get_concurrent_signal_groups(int phase_num) const
    {

        std::vector<int> concurrent_signal_groups;
        std::string concurrent_phases_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(phase_num);

        streets_snmp_cmd::snmp_response_obj concurrent_phase_data;
        auto request_type = streets_snmp_cmd::REQUEST_TYPE::GET;
        concurrent_phase_data.type = streets_snmp_cmd::RESPONSE_TYPE::STRING;
        snmp_client_worker_->process_snmp_request(concurrent_phases_oid, request_type, concurrent_phase_data);

        //extract phase numbers from strings
        for(auto con_phase :  concurrent_phase_data.val_string)
        {   if(con_phase != 0){
                concurrent_signal_groups.push_back(get_vehicle_signal_group_id(int(con_phase)));
            }
        }

        if(concurrent_signal_groups.empty())
        {
            SPDLOG_WARN("No concurrent signal groups found for phase {0}", phase_num);
        }

        return concurrent_signal_groups;
    }

    const std::unordered_map<int,int>& tsc_state::get_ped_phase_map()
    {
        return ped_phase_2signalgroup_map_;
    }

    const std::unordered_map<int,int>& tsc_state::get_vehicle_phase_map()
    {
        return vehicle_phase_2signalgroup_map_;
    }

    const std::unordered_map<int, int> & tsc_state::get_signal_group_map()
    {
        return signal_group_2vehiclephase_map_;
    }

    std::unordered_map<int, signal_group_state>& tsc_state::get_signal_group_state_map()
    {
        return signal_group_2tsc_state_map_;
    }

    std::unordered_map<int, int> tsc_state::get_signal_group_to_ped_phase_map()
    {
        std::unordered_map<int,int> signal_group_to_ped_phase;

        for(auto it = ped_phase_2signalgroup_map_.begin(); it != ped_phase_2signalgroup_map_.end();){
        
            signal_group_to_ped_phase.insert(std::make_pair(it->second,it->first));
        }
        
        return signal_group_to_ped_phase;
    }

}