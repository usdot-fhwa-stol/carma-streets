#include "snmp_client_exception.h"

namespace traffic_signal_controller_service {

    snmp_client_exception::snmp_client_exception(const std::string &msg): std::runtime_error(msg){};

    snmp_client_exception::~snmp_client_exception() = default;
}