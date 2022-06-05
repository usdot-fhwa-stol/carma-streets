# include <iostream>
# include <bits/stdc++.h> 
# include <chrono>
# include <sstream>
# include "snmp_client.h"


SnmpClient::SnmpClient()
{
    streets_service::streets_configuration::initialize_logger();
    
    // Get config parameters from json
    ip_ = streets_service::streets_configuration::get_string_config("ip");
    port_ = streets_service::streets_configuration::get_int_config("port");
    community_ = streets_service::streets_configuration::get_string_config("community");
    community_len_ = community_.length();
    snmp_version_ = streets_service::streets_configuration::get_string_config("snmp_version");
    timeout_ = streets_service::streets_configuration::get_int_config("timeout");

    SPDLOG_DEBUG("Target device IP address:", ip);
    SPDLOG_DEBUG("Target device NTCIP port", std::to_string(port));
    

    // Bring the IP address and port of the target SNMP device in the required form, which is "IPADDRESS:PORT":
    char ip_port[64];
    strcpy(ip_port,ip_.c_str());
    strcat(ip_port,":");
    strcat(ip_port,(std::to_string(port_)).c_str()); 
    
    init_snmp("carma_snmp");
    snmp_sess_init(&session);
    session.peername = ip_port;
    session.version = SNMP_VERSION_1;

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
        SPDLOG_DEBUG("Established session with device at ", ip_);
    }
    

}

void SnmpClientprocess_snmp_get_request(std::vector<std::string> input_oids){

    // Create pdu for the data
    pdu = snmp_pdu_create(SNMP_MSG_GET);

    // Read input OID into an OID variable:
    // net-snmp has several methods for creating an OID object
    // their documentation suggests using get_node. read_objid seems like a simpler approach
    // TO DO: investigate update to get_node
    bool failures = false;
    for input_oid : input_oids {
        
        if(!read_objid(input_oid.c_str(), OID, &OID_len)){
            // If oid cannot be created
            SPDLOG_ERROR("OID could not be created from input: ", input_oid);
            snmp_perror(input_oid.c_str())
            failures = true;
        }
        else{
            SPDLOG_DEBUG("Created OID for input: ", input_oid);
            snmp_add_null_var(pdu, OID, OID_len);
        }
    }

    if(failures){

    }

    // Send the request
    status = snmp_synch_response(ss, pdu, &response);
    if(status == STAT_SUCCESS) {
        if(response->errstat == SNMP_ERR_NOERROR){
            for(vars = response->variables; vars; vars = vars->next_variable){
                // Get value of variable depending on ASN.1 type
                // Variable could be a integer, string, bitstring, ojbid, counter : defined here https://github.com/net-snmp/net-snmp/blob/master/include/net-snmp/types.h
                uin8_t buf_array[vars->val_len];
                for(int i = 0; i < vars->val_len; ++i){
                    buf_array[i] = vars->val;
                }
            }
        }
    }



        
        

}

int SnmpClient::process_snmp_request(std::string request_type, std::string input_oid, int value)
{
    // Create pdu for the data
    if (request_type == "GET"){
        pdu = snmp_pdu_create(SNMP_MSG_GET);
    }
    else if (request_type == "SET"){
        pdu = snmp_pdu_create(SNMP_MSG_SET);
    }

    // Read input OID into an OID variable:
    // net-snmp has several methods for creating an OID object
    // their documentation suggests using get_node. read_objid seems like a simpler approach
    // TO DO: investigate update to get_node
    read_objid(input_oid.c_str(), OID, &OID_len);
    

    // For get request, add null variable
    if (request_type == "GET"){
        snmp_add_null_var(pdu, OID, OID_len);
    }
    else if (request_type == "set"){
        snmp_add_var(pdu, OID, OID_len, 'i', std::to_string(value).c_str());
    }

    // Send the request
    status = snmp_synch_response(ss, pdu, &response);

    // Handle response
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR){

        if (request_type == "GET"){
            int out[50]{};
            int i{};
            for(vars = response->variables; vars; vars = vars->next_variable)
            {
                int **aa{};
                aa = (int*)vars->val.integer;
                out[i++] = *aa;
                value = out[0];
            }
            SPDLOG_DEBUG("Success in GET for OID: ", input_oid, " ; Value = ", std::to_string(value));
        }
        else{
            SPDLOG_DEBUG("Success in SET for OID: ", input_oid , " ; Value = ", std::to_string(value));
        }
    }

    // Free response
    if(response){
        snmp_free_pdu(response);
    }

}

SnmpClient::~SnmpClient()
{
    snmp_close(ss);
}