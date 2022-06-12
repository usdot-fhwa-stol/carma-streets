# include <iostream>
#include "snmp_client.h"

int main()
{
    
    std::string setCustomMibsCommand = "export MIBS=ALL";
    system(setCustomMibsCommand.c_str()); 

    SnmpClient worker();
    streets_service::streets_configuration::initialize_logger();

    // Load config parameters from json
    worker.ip_ = streets_service::streets_configuration::get_string_config("ip");
    worker.port_ = streets_service::streets_configuration::get_int_config("port");
    worker.community_ = streets_service::streets_configuration::get_string_config("community");
    worker.community_len_ = community_.length();
    worker.snmp_version_ = streets_service::streets_configuration::get_string_config("snmp_version");
    
    std::string input_oid = ".1.3.6.1.4.1.1206.4.2.6.3.1";
    worker.process_snmp_get_request(input_oid);

    return 0;
}