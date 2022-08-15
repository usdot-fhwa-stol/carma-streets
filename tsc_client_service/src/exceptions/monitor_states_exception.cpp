#include "monitor_states_exception.h"

namespace traffic_signal_controller_service {

    monitor_states_exception::monitor_states_exception(const std::string &msg): std::runtime_error(msg){};

    monitor_states_exception::~monitor_states_exception() = default;
}