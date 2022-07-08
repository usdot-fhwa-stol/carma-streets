# include <iostream>
# include <bits/stdc++.h> 
# include <chrono>
# include <sstream>
# include "snmp_client.h"
# include "ntcip_oids.h"
#include "spat_worker_exception.h"

namespace traffic_signal_controller_service
{
    
snmp_client::snmp_client(const std::string& ip, const int& port, const std::string& community, int snmp_version, int timeout)
    : ip_(ip), port_(port), community_(community),snmp_version_(snmp_version), timeout_(timeout)
{
    
    SPDLOG_DEBUG("Starting SNMP Client");
    SPDLOG_DEBUG("Target device IP address: {0}", ip_);
    SPDLOG_INFO("Target device NTCIP port: {0}", port_);
    

    // Bring the IP address and port of the target SNMP device in the required form, which is "IPADDRESS:PORT":
    std::string ip_port_string = ip_ + ":" + std::to_string(port_);    
    char* ip_port = &ip_port_string[0];
    
    init_snmp("carma_snmp");
    snmp_sess_init(&session);
    session.peername = ip_port;
    session.version = snmp_version_;

    // Establish the session parameters.
    char community_char[community_.length()];
    std::copy(community_.begin(), community_.end(), community_char);
    unsigned char* comm = reinterpret_cast<unsigned char*>(community_char);
    
    session.community = comm;
    session.community_len = community_.length();
    session.timeout = timeout_;

    ss = snmp_open(&session);

    if (ss == nullptr)
    {
        SPDLOG_ERROR("Failed to establish session with target device");
        snmp_sess_perror("snmpget", &session);
    }
    else
    {
        SPDLOG_INFO("Established session with device at {0}", ip_);
    }
    
    // Map signal group ids and phase nums
    //Get phase number given a signal group id
    
    int64_t max_channels_in_tsc = get_max_channels();
    std::vector<int> vehicle_phase_channels = get_vehicle_phase_channels(max_channels_in_tsc);
    map_phase_and_signalgroup(vehicle_phase_channels);

}

bool snmp_client::process_snmp_request(const std::string& input_oid, const std::string& request_type, int64_t& value){

    /*Structure to hold response from the remote host*/
    snmp_pdu *response;

    // Create pdu for the data
    if (request_type == "GET")
    {
        SPDLOG_DEBUG("Attemping to GET value for: {0}", input_oid);
        pdu = snmp_pdu_create(SNMP_MSG_GET);
    }
    else if (request_type == "SET")
    {
        SPDLOG_DEBUG("Attemping to SET value for {0}", input_oid, " to {1}", value);
        pdu = snmp_pdu_create(SNMP_MSG_SET);
    }
    else{
        SPDLOG_ERROR("Invalid request type, method accepts only GET and SET");
    }

    // Read input OID into an OID variable:
    // net-snmp has several methods for creating an OID object
    // their documentation suggests using get_node. read_objid seems like a simpler approach
    // TO DO: investigate update to get_node
    if(!read_objid(input_oid.c_str(), OID, &OID_len)){
        // If oid cannot be created
        SPDLOG_ERROR("OID could not be created from input: {0}", input_oid);
        return false;
        
    }
    else{
        
        if(request_type == "GET")
        {
            // Add OID to pdu for get request
            snmp_add_null_var(pdu, OID, OID_len);
        }
        else if(request_type == "SET")
        {
            snmp_add_var(pdu, OID, OID_len, 'i', (std::to_string(value)).c_str());
        }

        SPDLOG_INFO("Created OID for input: {0}", input_oid);
    }

    // Send the request
    int status = snmp_synch_response(ss, pdu, &response);

    // Check response
    if(status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        
        SPDLOG_INFO("STAT_SUCCESS, received a response");
        
        if(request_type == "GET"){
            for(auto vars = response->variables; vars; vars = vars->next_variable){
                // Get value of variable depending on ASN.1 type
                // Variable could be a integer, string, bitstring, ojbid, counter : defined here https://github.com/net-snmp/net-snmp/blob/master/include/net-snmp/types.h
                // get Integer value
                if(vars->type == ASN_INTEGER){
                    if(vars->val.integer){
                        value = *vars->val.integer;
                        SPDLOG_INFO("Integer value in object: {0}", value);
                    }
                    else{
                        SPDLOG_ERROR("Response specifies type integer, but no integer value found");
                        return false;
                    }
                    
                }
                else{
                    SPDLOG_ERROR("Received a message type which isn't an Integer");
                    return false;
                }
            }
        }
        else if(request_type == "SET"){
            SPDLOG_DEBUG("Success in SET for OID: {0}", input_oid," ; Value: {1}", value);
        }
    
    }
    else 
    {
        log_error(status, request_type, response);
        return false;
    }

    if (response){
        snmp_free_pdu(response);
        OID_len = MAX_OID_LEN;
    }
    
    return true;
}


void snmp_client::log_error(const int& status, const std::string& request_type, snmp_pdu *response)
{

    if (status == STAT_SUCCESS)
    {
        SPDLOG_ERROR("Variable type: {0}",response->variables->type);
        SPDLOG_ERROR("Error in packet.{0} ", static_cast<std::string>(snmp_errstring(static_cast<int>(response->errstat))));
    }
    else if (status == STAT_TIMEOUT){ 
    
        SPDLOG_ERROR("Timeout, no response from server");
    }
    else{
    
        SPDLOG_ERROR("Unknown SNMP Error for {0}", request_type);
    }
    
}

int64_t& snmp_client::get_max_channels(){
    std::string request_type = "GET";
    int64_t max_channels_in_tsc = 0;

    try{
        process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type, max_channels_in_tsc);
    }
    catch ( const traffic_signal_controller_service::spat_worker_exception &e) {
        SPDLOG_ERROR("Failed to get max channels : {0} ", e.what());
    }

    return max_channels_in_tsc;
}

std::vector<int>& snmp_client::get_vehicle_phase_channels(int64_t& max_channels){
    std::vector<int> vehicle_phase_channels;
    // Loop through channel control types and add channels with vehicle phase to list
    int64_t vehicle_control_type  = 0;
    std::string request_type = "GET";
    for(int channel_num = 0; channel_num < max_channels; ++channel_num)
    {
        std::string control_type_parameter_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(channel_num);

        try{
            process_snmp_request(control_type_parameter_oid, request_type, vehicle_control_type);
        }
        catch ( const traffic_signal_controller_service::spat_worker_exception &e) {
            SPDLOG_ERROR("Failed to get channel control type : {0} ", e.what());
        }
        
        if(vehicle_control_type == 2)
        {
            vehicle_phase_channels.push_back(channel_num);
        }

    }

    return vehicle_phase_channels;
}

void snmp_client::map_phase_and_signalgroup(std::vector<int>& vehicle_phase_channels)
{
    // According to NTCIP 1202 v03 documentation Signal Group ID in a SPAT message is the Channel Number from TSC

    // Get phases associated with vehicle phase channels
    std::string request_type = "GET";
    for(int channel : vehicle_phase_channels)
    {
        int64_t phase_num = 0;
        std::string control_source_parameter_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(channel);
        process_snmp_request(control_source_parameter_oid, request_type, phase_num);

        // According to NTCIP 1202 v03 returned value of 0 here would mean a phase is not associated with the channel
        if(phase_num != 0)
        {
            phase_num_map_.insert(std::make_pair(channel, phase_num));
            signal_group_map_.insert(std::make_pair(phase_num, channel));
        }
    }
}

}