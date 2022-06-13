# include <iostream>
# include <bits/stdc++.h> 
# include <chrono>
# include <sstream>
# include "snmp_client.h"


SnmpClient::SnmpClient(std::string ip, int port, std::string community, int community_len, int snmp_version, int timeout)
{
    SPDLOG_DEBUG("Starting SNMP Client");
    SPDLOG_DEBUG("Target device IP address:", ip_);
    SPDLOG_DEBUG("Target device NTCIP port", std::to_string(port_));
    ip_ = ip;
    port_ = port;
    community_ = community;
    community_len_ = community_len;
    timeout_ = timeout;
    

    // Bring the IP address and port of the target SNMP device in the required form, which is "IPADDRESS:PORT":
    char ip_port[64];
    strcpy(ip_port,ip_.c_str());
    strcat(ip_port,":");
    strcat(ip_port,(std::to_string(port_)).c_str()); 
    
    init_snmp("carma_snmp");
    snmp_sess_init(&session);
    session.peername = ip_port;
    session.version = snmp_version_;

    // Establish the session parameters.
    unsigned char comm[] = "public";
    session.community = comm;
    session.community_len = strlen((const char *)session.community);

    ss = snmp_open(&session);

    if (ss == NULL)
    {
        SPDLOG_ERROR("Failed to establish session with target device");
        snmp_sess_perror("snmpget", &session);
    }
    else
    {
        SPDLOG_INFO("Established session with device at ", ip_);
    }
    

}

void SnmpClient::process_snmp_get_request(std::string input_oid){

    // Create pdu for the data
    pdu = snmp_pdu_create(SNMP_MSG_GET);

    // Read input OID into an OID variable:
    // net-snmp has several methods for creating an OID object
    // their documentation suggests using get_node. read_objid seems like a simpler approach
    // TO DO: investigate update to get_node
        
    if(!read_objid(input_oid.c_str(), OID, &OID_len)){
        // If oid cannot be created
        SPDLOG_ERROR("OID could not be created from input: ", input_oid);
        snmp_perror(input_oid.c_str());
    }
    else{
        SPDLOG_INFO("Created OID for input: ", input_oid);
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
                SPDLOG_DEBUG("Integer value in object: ", vars->val.integer);
            }
            else{
                SPDLOG_INFO("Received a message type which isn't an Integer");
            }
        }
        
    }
    else{
        
        SPDLOG_ERROR("Error in response");
        
        if (status == STAT_SUCCESS)
        {
            SPDLOG_ERROR("Error in packet.", static_cast<std::string>(snmp_errstring(static_cast<int>(response->errstat))));
        }
        else if (status == STAT_TIMEOUT)
        { 
            SPDLOG_ERROR("Timeout, no response from ", session.peername);
        }
        else{
            snmp_sess_perror("snmpget", ss);
            SPDLOG_ERROR("Unknown SNMP Error");
        }
        
    }
    
    

    if (response){
        snmp_free_pdu(response);
    }
        
}

void SnmpClient::process_snmp_set_request(std::string input_oid, int value){

    SPDLOG_DEBUG("Attemping to SET value for ", input_oid, " to ", value);
    // Create pdu for the data
    pdu = snmp_pdu_create(SNMP_MSG_SET);

    if(!read_objid(input_oid.c_str(), OID, &OID_len)){
        // If oid cannot be created
        SPDLOG_ERROR("OID could not be created from input: ", input_oid);
        snmp_perror(input_oid.c_str());
    }
    else{
        SPDLOG_INFO("Created OID for input: ", input_oid);
        snmp_add_var(pdu, OID, OID_len, 'i', (std::to_string(value)).c_str());
    }

    status = snmp_synch_response(ss, pdu, &response);

    if (status = STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR){
        SPDLOG_DEBUG("Success in SET for OID:", input_oid," ; Value: ", value);
    }
    else{
            SPDLOG_ERROR("Error in response");
            
            if (status == STAT_SUCCESS)
            {
                SPDLOG_ERROR("Error in packet.", static_cast<std::string>(snmp_errstring(static_cast<int>(response->errstat))));
            }
            else if (status == STAT_TIMEOUT){ 
                SPDLOG_ERROR("Timeout, no response from ", session.peername);
            }
            else{
                snmp_sess_perror("snmpset", ss);
                SPDLOG_ERROR("Unknown SNMP Error");
            }
            
    }

    if (response){
        snmp_free_pdu(response);
    }
}



SnmpClient::~SnmpClient()
{
    SPDLOG_INFO("Closing snmp session");
    snmp_close(ss);
}