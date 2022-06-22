# include <iostream>
# include <bits/stdc++.h> 
# include <chrono>
# include <sstream>
# include "snmp_client.h"


TrafficSignalControllerService::TrafficSignalControllerService(const std::string& ip, const int& port, const std::string& community, int snmp_version, int timeout)
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
    unsigned char comm[] = "public";
    session.community = comm;
    session.community_len = strlen((const char *)session.community);
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
    

}

bool TrafficSignalControllerService::process_snmp_get_request(std::string input_oid, int& integer_response){

    // Create pdu for the data
    pdu = snmp_pdu_create(SNMP_MSG_GET);

    // Read input OID into an OID variable:
    // net-snmp has several methods for creating an OID object
    // their documentation suggests using get_node. read_objid seems like a simpler approach
    // TO DO: investigate update to get_node
        
    if(!read_objid(input_oid.c_str(), OID, &OID_len)){
        // If oid cannot be created
        SPDLOG_ERROR("OID could not be created from input: {0}", input_oid);
        snmp_perror(input_oid.c_str());
        return false;
    }
    else{
        SPDLOG_INFO("Created OID for input: {0}", input_oid);
        // Add OID to pdu for get request
        snmp_add_null_var(pdu, OID, OID_len);
    }
    

    // Send the request
    status = snmp_synch_response(ss, pdu, &response);

    // Check response
    if(status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        
        SPDLOG_INFO("STAT_SUCCESS, received a response");
        for(auto vars = response->variables; vars; vars = vars->next_variable){
            // Get value of variable depending on ASN.1 type
            // Variable could be a integer, string, bitstring, ojbid, counter : defined here https://github.com/net-snmp/net-snmp/blob/master/include/net-snmp/types.h

            // get Integer value
            if(vars->type == ASN_INTEGER){
                integer_response = *vars->val.integer;
                SPDLOG_INFO("Integer value in object: {0}", integer_response);
            }
            else{
                SPDLOG_ERROR("Received a message type which isn't an Integer");
                return false;
            }
        }
        
    }
    else{
        
        std::string request_type = "GET";
        log_error(status, request_type);
        
        return false;
        
    }

    if (response){
        snmp_free_pdu(response);
        OID_len = MAX_OID_LEN;
    }
    
    return true;
}

bool TrafficSignalControllerService::process_snmp_set_request(std::string input_oid, int value){

    SPDLOG_DEBUG("Attemping to SET value for {0}", input_oid, " to {1}", value);
    // Create pdu for the data
    pdu = snmp_pdu_create(SNMP_MSG_SET);

    // Read input OID into an OID variable:
    if(!read_objid(input_oid.c_str(), OID, &OID_len)){
        // If oid cannot be created
        SPDLOG_ERROR("OID could not be created from input: {0}", input_oid);
        snmp_perror(input_oid.c_str());
        return false;
    }
    else{
        
        if (snmp_parse_oid(input_oid.c_str(), OID, &OID_len) == nullptr){
            SPDLOG_ERROR("Couldn't parse oid");
            return false;
        }
        // Add OID to pdu for set request
        snmp_add_var(pdu, OID, OID_len, 'i', (std::to_string(value)).c_str());
        
        SPDLOG_INFO("Created OID for input: {0}", input_oid);
        
    }

    status = snmp_synch_response(ss, pdu, &response);

    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR){
        SPDLOG_DEBUG("Success in SET for OID: {0}", input_oid," ; Value: {1}", value);
    }
    else{
        std::string request_type = "SET";
        log_error(status, request_type);

        return false;
            
    }

    if (response){
        snmp_free_pdu(response);
        OID_len = MAX_OID_LEN;
    }
    return true;
}

void TrafficSignalControllerService::log_error(int& status, std::string& request_type)
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
    
        if(request_type == "SET"){
            snmp_sess_perror("snmpset", ss);//Logs error to net-snmp logfile
        }
        else if(request_type == "GET"){
            snmp_sess_perror("snmpget", ss); ////Logs error to net-snmp logfile
        }
        SPDLOG_ERROR("Unknown SNMP Error");
    }
    
}