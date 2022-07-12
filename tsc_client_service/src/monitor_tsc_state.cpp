#include "monitor_tsc_state.h"
# include "ntcip_oids.h"

namespace traffic_signal_controller_service
{
    tsc_state::tsc_state(std::shared_ptr<snmp_client> snmp_client) 
    {
        snmp_client_worker_ = snmp_client;
        // Map signal group ids and phase nums
        //Get phase number given a signal group id
        int64_t max_channels_in_tsc = get_max_channels();
        std::vector<int> vehicle_phase_channels = get_vehicle_phase_channels(max_channels_in_tsc);
        map_phase_and_signalgroup(vehicle_phase_channels);
    }

    int tsc_state::get_max_channels(){
        int request_type = request_type::GET;
        int64_t max_channels_in_tsc = 0;

        if(!snmp_client_worker_->process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type, max_channels_in_tsc))
        {
            SPDLOG_ERROR("Failed to get max channels");
        }

        return (int) max_channels_in_tsc;
    }

    std::vector<int> tsc_state::get_vehicle_phase_channels(int max_channels){
        std::vector<int> vehicle_phase_channels;
        // Loop through channel control types and add channels with vehicle phase to list
        int64_t vehicle_control_type  = 0;
        int request_type = request_type::GET;
        for(int channel_num = 0; channel_num < max_channels; ++channel_num)
        {
            std::string control_type_parameter_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(channel_num);

            if(!snmp_client_worker_->process_snmp_request(control_type_parameter_oid, request_type, vehicle_control_type))
            {
                SPDLOG_ERROR("Failed to get channel control type");
            }
            
            if(vehicle_control_type == 2)
            {
                vehicle_phase_channels.push_back(channel_num);
            }

        }

        if(vehicle_phase_channels.empty()){
            SPDLOG_WARN("Found no active vehicle phases");
        }

        return vehicle_phase_channels;
    }

    void tsc_state::map_phase_and_signalgroup(const std::vector<int>& vehicle_phase_channels)
    {
        // According to NTCIP 1202 v03 documentation Signal Group ID in a SPAT message is the Channel Number from TSC

        // Get phases associated with vehicle phase channels
        int request_type = request_type::GET;
        for(int channel : vehicle_phase_channels)
        {
            int64_t phase_num = 0;
            std::string control_source_parameter_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(channel);
            snmp_client_worker_->process_snmp_request(control_source_parameter_oid, request_type, phase_num);

            // According to NTCIP 1202 v03 returned value of 0 here would mean a phase is not associated with the channel
            if(phase_num != 0)
            {
                phase_num_map_.insert(std::make_pair(channel, phase_num));
                signal_group_phase_map_.insert(std::make_pair(phase_num, channel));
                SPDLOG_DEBUG("Found mapping between signal group: {0}", channel, " and phase num: {1}", phase_num );
            }
        }
        
    }

    int tsc_state::get_min_green(int signal_group_id)
    {
        int request_type = request_type::GET;
        int phase_num = 0;
        try{
            signal_group_phase_map_.find(signal_group_id) != signal_group_phase_map_.end();
        }catch (std::invalid_argument const& ex){
            std::runtime_error("Couldn't find a phase associated with signal group id: " + std::to_string(signal_group_id));
        }
        
        phase_num = signal_group_phase_map_[signal_group_id];

        std::string min_green_parameter_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(phase_num);
        int64_t min_green_val = 0;
        snmp_client_worker_->process_snmp_request(min_green_parameter_oid, request_type, min_green_val);

        return (int) min_green_val;
    }

    int tsc_state::get_max_green(int signal_group_id)
    {
        int request_type = request_type::GET;
        int phase_num = 0;
        
        try{
            signal_group_phase_map_.find(signal_group_id) != signal_group_phase_map_.end();
        }catch (std::invalid_argument const& ex){
            std::runtime_error("Couldn't find a phase associated with signal group id: " + std::to_string(signal_group_id));
        }
        
        phase_num = signal_group_phase_map_[signal_group_id];

        std::string max_green_parameter_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(phase_num);
        int64_t max_green_val = 0;
        snmp_client_worker_->process_snmp_request(max_green_parameter_oid, request_type, max_green_val);

        return (int) max_green_val;
    }

    int tsc_state::get_yellow_duration(int phase_num)
    {
        int request_type = request_type::GET;
        std::string yellow_duration_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(phase_num);
        int64_t yellow_duration_val = 0;
        snmp_client_worker_ ->process_snmp_request(yellow_duration_oid, request_type, yellow_duration_val);

        return (int) yellow_duration_val;
    }

    int tsc_state::get_red_duration(int phase_num)
    {
        int request_type = request_type::GET;
        //TODO: At this point the phase sequence should be filled
    }

    std::vector<int> tsc_state::phase_seq(int phase_num)
    {
        // Read sequence 1 data for first 2 rings
        int request_type = request_type::GET;
        std::string phase_seq_in_ring1 = "";
        std::string phase_seq_in_ring2 = "";

        std::string phase_seq_oid_ring1= ntcip_oids::SEQUENCE_DATA + "." + "1" + "1"; //Sequence 1 ring 1
        std::string phase_seq_oid_ring2 = ntcip_oids::SEQUENCE_DATA + "." + "1" + "2"; //Sequence 1 ring 2
        // TODO: Complete impl
        // snmp_client_worker_->process_snmp_request(phase_seq_oid_ring1, request_type, phase_seq_in_ring1);
        // snmp_client_worker_->process_snmp_request(phase_seq_oid_ring2, request_type, phase_seq_in_ring2);





    }
}