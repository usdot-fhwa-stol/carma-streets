#include <iostream>
#include "snmp_client.h"
#include "spat_receive_worker.h"

int main()
{
    
    std::string setCustomMibsCommand = "export MIBS=ALL";
    system(setCustomMibsCommand.c_str()); 

    streets_service::streets_configuration::initialize_logger();

    // Load config parameters from json
    std::string ip = streets_service::streets_configuration::get_string_config("ip");
    std::string local_ip = streets_service::streets_configuration::get_string_config("local_ip");
    int port = streets_service::streets_configuration::get_int_config("port");
    std::string community = streets_service::streets_configuration::get_string_config("community");
    int community_len = community.length();
    int snmp_version = streets_service::streets_configuration::get_int_config("snmp_version");
    int timeout = streets_service::streets_configuration::get_int_config("timeout");
    int socketTimeout = streets_service::streets_configuration::get_int_config("socket_timeout");

    TrafficSignalControllerService worker(ip, port, community, community_len, snmp_version, timeout);
    
    std::string input_oid = "1.3.6.1.4.1.1206.4.2.1.1.2.1.21.2";
    worker.process_snmp_get_request(input_oid);
    input_oid = "1.3.6.1.4.1.1206.4.2.1.1.2.1.6.2";
    int set_value = 10;
    worker.process_snmp_set_request(input_oid, set_value);

    //enable spat OID on tsc
    std::string enable_spat_oid = "1.3.6.1.4.1.1206.3.5.2.9.44.1.0";
    int enable_spat_value = 2;
    worker.process_snmp_set_request(enable_spat_oid, enable_spat_value);
    
    SpatWorker spatWorker(local_ip, port, socketTimeout);
    spatWorker.createSocket();

    return 0;
}