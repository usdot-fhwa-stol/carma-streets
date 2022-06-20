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
    

    return 0;
}