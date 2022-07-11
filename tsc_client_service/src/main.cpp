#include <iostream>
#include "snmp_client.h"
#include "spat_worker.h"
#include "ntcip_oids.h"
#include "spat_worker_exception.h"

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
    int request_type = traffic_signal_controller_service::request_type::SET;
    int64_t enable_spat_value = 2;
    worker.process_snmp_request(ntcip_oids::ENABLE_SPAT_OID, request_type, enable_spat_value);

    //instantiate spat receive worker
    traffic_signal_controller_service::spat_worker spat_worker(local_ip, local_port, socketTimeout);
    // Create SPaT UDP socket
    try {
        spat_worker.listen_udp_spat();
    }
    catch ( const traffic_signal_controller_service::spat_worker_exception &e) {
        SPDLOG_ERROR("Failed to create UDP socket for NTCIP SPaT data : {0} ", e.what());
    }
    return 0;
}