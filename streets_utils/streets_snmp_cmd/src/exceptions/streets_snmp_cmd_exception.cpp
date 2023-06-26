#include "streets_snmp_cmd_exception.h"

namespace streets_snmp_cmd {

    streets_snmp_cmd_exception::streets_snmp_cmd_exception(const std::string &msg): std::runtime_error(msg){};

    streets_snmp_cmd_exception::~streets_snmp_cmd_exception() = default;
}