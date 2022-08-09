#include "monitor_tsc_state.h"
# include "ntcip_oids.h"

namespace traffic_signal_controller_service
{
    tsc_state::tsc_state(std::shared_ptr<snmp_client> snmp_client) : snmp_client_worker_(snmp_client)
    {
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
            state.concurrent_phases = get_concurrent_phases(phase_num);
            signal_group_state_map_.insert(std::make_pair(signal_group.first, state));
        }

        // Loop through states once other state parameters are defined to get the red duration
        for(auto& state : signal_group_state_map_)
        {
            state.second.red_duration = get_red_duration(state.second.phase_num);
        }


        // Print signal_group map
        for (const auto& phase : signal_group_phase_map_)
        {
            SPDLOG_DEBUG("Signal group id: {0} phase: {1}", phase.first, phase.second);
        }
        // Print state map
        for (const auto& state : signal_group_state_map_)
        {
            SPDLOG_DEBUG("Signal group id: {0}", state.first);
            SPDLOG_DEBUG("Phase num: {0}", state.second.phase_num);
            SPDLOG_DEBUG("Max green: {0}", state.second.max_green);
            SPDLOG_DEBUG("Min green: {0}", state.second.min_green);
            SPDLOG_DEBUG("Green Duration: {0}", state.second.green_duration);
            SPDLOG_DEBUG("yellow duration: {0}", state.second.yellow_duration);
            SPDLOG_DEBUG("Red clearance: {0}", state.second.red_clearance);
            SPDLOG_DEBUG("Red duration: {0}", state.second.red_duration);

            SPDLOG_DEBUG("Phase sequence:");
            for(auto phase : state.second.phase_seq){
                SPDLOG_DEBUG("{0}", phase);
            }

            SPDLOG_DEBUG("Concurrent Phases:");
            for(auto phase : state.second.concurrent_phases){
                SPDLOG_DEBUG("{0}", phase);
            }
            
        }
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

        SPDLOG_ERROR("No following phases found");
        return sequence;

    }

    int tsc_state::get_max_channels() const {
        request_type request_type = request_type::GET;
        snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.type = snmp_response_obj::response_type::INTEGER;

        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type, max_channels_in_tsc))
        {
            SPDLOG_ERROR("Failed to get max channels");
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
                SPDLOG_ERROR("Failed to get channel control type");
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

    int tsc_state::get_min_green(int phase_num) const
    {
        request_type request_type = request_type::GET;
        std::string min_green_parameter_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(phase_num);

        snmp_response_obj min_green;
        min_green.type = snmp_response_obj::response_type::INTEGER;

        snmp_client_worker_->process_snmp_request(min_green_parameter_oid, request_type, min_green);

        return (int) min_green.val_int;
    }

    int tsc_state::get_max_green(int phase_num) const
    {
        request_type request_type = request_type::GET;

        std::string max_green_parameter_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(phase_num);
        snmp_response_obj max_green;
        max_green.type = snmp_response_obj::response_type::INTEGER;
        
        snmp_client_worker_->process_snmp_request(max_green_parameter_oid, request_type, max_green);

        return (int) max_green.val_int;
    }

    int tsc_state::get_yellow_duration(int phase_num) const
    {
        request_type request_type = request_type::GET;
        std::string yellow_duration_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(phase_num);

        snmp_response_obj yellow_duration;
        yellow_duration.type = snmp_response_obj::response_type::INTEGER;
        
        snmp_client_worker_ ->process_snmp_request(yellow_duration_oid, request_type, yellow_duration);

        return (int) yellow_duration.val_int / 10; //Divide by 10 since NTCIP returned value is in tenths of seconds
    }

    int tsc_state::get_red_clearance(int phase_num) const
    {
        request_type get_request = request_type::GET;
        std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(phase_num);

        snmp_response_obj red_clearance;
        red_clearance.type = snmp_response_obj::response_type::INTEGER;

        snmp_client_worker_->process_snmp_request(red_clearance_oid, get_request, red_clearance);

        return (int) red_clearance.val_int / 10; //Divide by 10 since NTCIP returned value is in tenths of seconds
    }

    int tsc_state::get_red_duration(int phase_num)
    {
        //red duration is calculated as red_clearance time for current phase + (green duration + yellow duration + red_clearance) for other phases in sequence

        // Find signal state associated with phase num
        int current_signal_group;
        if(vehicle_phase_num_map_.find(phase_num) != vehicle_phase_num_map_.end())
        {
            current_signal_group = vehicle_phase_num_map_[phase_num];
        }
        else{
            SPDLOG_ERROR("No signal state associated with phase {0}", phase_num);
            return 0;
        }
        auto current_signal_group_state = signal_group_state_map_[current_signal_group];
        int red_duration = current_signal_group_state.red_clearance;
        
        for(auto phase : current_signal_group_state.phase_seq)
        {
            // Find state params for each phase in seq
            int seq_signal_group;
            if(vehicle_phase_num_map_.find(phase_num) != vehicle_phase_num_map_.end())
            {
                seq_signal_group = vehicle_phase_num_map_[phase];
            }
            else{
                SPDLOG_ERROR("No signal state associated with phase {0}", phase);
                return 0;
            }
            auto seq_signal_group_state = signal_group_state_map_[seq_signal_group];
            if(phase == phase_num)
            {
                // Only add clearance time for current phase
                red_duration += seq_signal_group_state.red_clearance;
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
        // Read sequence 1 data for first 2 rings
        request_type request_type = request_type::GET;
        std::string phase_seq_oid= ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(ring_num); //Sequence 1 for ring

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

    std::vector<int> tsc_state::get_concurrent_phases(int phase_num) const
    {

        std::vector<int> concurrent_phases;
        std::string concurrent_phases_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(phase_num);

        snmp_response_obj concurrent_phase_data;
        request_type request_type = request_type::GET;
        concurrent_phase_data.type = snmp_response_obj::response_type::STRING;
        snmp_client_worker_->process_snmp_request(concurrent_phases_oid, request_type, concurrent_phase_data);

        //extract phase numbers from strings
        for(auto con_phase :  concurrent_phase_data.val_string)
        {   
            concurrent_phases.push_back(int(con_phase));
            
        }

        if(concurrent_phases.empty())
        {
            SPDLOG_WARN("No phases found in sequence for phase {0}", phase_num);
        }

        return concurrent_phases;
    }

    const std::unordered_map<int,int>& tsc_state::get_ped_phase_map()
    {
        return ped_phase_num_map_;
    }

    const std::unordered_map<int,int>& tsc_state::get_vehicle_phase_map()
    {
        return vehicle_phase_num_map_;
    }
}