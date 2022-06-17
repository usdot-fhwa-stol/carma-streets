# include <iostream>
#include "snmp_client.h"

int main()
{
    
    std::string setCustomMibsCommand = "export MIBS=ALL";
    system(setCustomMibsCommand.c_str()); 

    streets_service::streets_configuration::initialize_logger();

    // Load config parameters from json
    std::string ip = streets_service::streets_configuration::get_string_config("ip");
    int port = streets_service::streets_configuration::get_int_config("port");
    std::string community = streets_service::streets_configuration::get_string_config("community");
    int snmp_version = streets_service::streets_configuration::get_int_config("snmp_version");
    int timeout = streets_service::streets_configuration::get_int_config("timeout");

    TrafficSignalControllerService worker(ip, port, community, snmp_version, timeout);
    
    std::string input_oid = "1.3.6.1.4.1.1206.4.2.1.1.2.1.21.2";
    if(!worker.process_snmp_get_request(input_oid)){
        SPDLOG_ERROR("Couldn't complete get request");
    }
    
    input_oid = "1.3.6.1.4.1.1206.4.2.1.1.2.1.6.2";
    int set_value = 10;
    if(!worker.process_snmp_set_request(input_oid, set_value)){
        SPDLOG_ERROR("Couldn't complete set request");
    }

    return 0;
}