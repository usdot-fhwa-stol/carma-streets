#include <iostream>
#include "snmp_client.h"
#include "spat_worker.h"

int main()
{
    
    std::string setCustomMibsCommand = "export MIBS=ALL";
    system(setCustomMibsCommand.c_str()); 

    streets_service::streets_configuration::initialize_logger();

    // Load config parameters from json
    std::string target_ip = streets_service::streets_configuration::get_string_config("target_ip");
    int target_port = streets_service::streets_configuration::get_int_config("target_port");
    std::string community = streets_service::streets_configuration::get_string_config("community");
    int snmp_version = streets_service::streets_configuration::get_int_config("snmp_version");
    int timeout = streets_service::streets_configuration::get_int_config("timeout");
    std::string local_ip = streets_service::streets_configuration::get_string_config("local_ip");
    int local_port = streets_service::streets_configuration::get_int_config("local_udp_port");
    int socketTimeout = streets_service::streets_configuration::get_int_config("socket_timeout");

    traffic_signal_controller_service::snmp_client worker(target_ip, target_port, community, snmp_version, timeout);
    
    //enable spat udp stream on tsc
    std::string enable_spat_oid = "1.3.6.1.4.1.1206.3.5.2.9.44.1.0";
    std::string request_type = "SET";
    int64_t enable_spat_value = 2;
    worker.process_snmp_request(enable_spat_oid, request_type, enable_spat_value);

    //instantiate spat receive worker
    traffic_signal_controller_service::spat_worker spatWorker(local_ip, local_port, socketTimeout);
    // We can implement a retry loop here if we would like.
    spatWorker.listen_udp_spat();

    return 0;
}