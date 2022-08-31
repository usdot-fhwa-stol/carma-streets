#include "monitor_desired_phase_plan_exception.h"

namespace traffic_signal_controller_service {

    monitor_desired_phase_plan_exception::monitor_desired_phase_plan_exception(const std::string &msg): std::runtime_error(msg){};

    monitor_desired_phase_plan_exception::~monitor_desired_phase_plan_exception() = default;
}