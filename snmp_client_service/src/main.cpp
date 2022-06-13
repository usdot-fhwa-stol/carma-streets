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
    int community_len = community.length();
    int snmp_version = streets_service::streets_configuration::get_int_config("snmp_version");
    int timeout = streets_service::streets_configuration::get_int_config("timeout");

    SnmpClient worker(ip, port, community, community_len, snmp_version, timeout);
    
    std::string input_oid = "1.3.6.1.4.1.1206.4.2.1.1.2.1.21.2";
    worker.process_snmp_get_request(input_oid);

    worker.process_snmp_set_request(input_oid, 1057);

    return 0;
}