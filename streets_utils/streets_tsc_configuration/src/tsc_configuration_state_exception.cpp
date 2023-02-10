#include "tsc_configuration_state_exception.h"

namespace streets_tsc_configuration {

    tsc_configuration_state_exception::tsc_configuration_state_exception(const std::string &msg): std::runtime_error(msg){};

    tsc_configuration_state_exception::~tsc_configuration_state_exception() = default;
}