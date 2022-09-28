#include "streets_desired_phase_plan_arbitrator_exception.h"

namespace streets_desired_phase_plan_arbitrator {

    streets_desired_phase_plan_arbitrator_exception::streets_desired_phase_plan_arbitrator_exception(const std::string &msg): std::runtime_error(msg){};

    streets_desired_phase_plan_arbitrator_exception::~streets_desired_phase_plan_arbitrator_exception() = default;
}