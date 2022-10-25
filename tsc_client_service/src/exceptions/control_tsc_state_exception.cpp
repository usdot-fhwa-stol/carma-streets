#include "control_tsc_state_exception.h"

namespace traffic_signal_controller_service
{
    control_tsc_state_exception::control_tsc_state_exception(const std::string &msg): std::runtime_error(msg){};

    control_tsc_state_exception::~control_tsc_state_exception() = default;

}