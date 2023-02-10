#include "streets_desired_phase_plan_exception.h"

namespace streets_desired_phase_plan {

    streets_desired_phase_plan_exception::streets_desired_phase_plan_exception(const std::string &msg): std::runtime_error(msg){};

    streets_desired_phase_plan_exception::~streets_desired_phase_plan_exception() = default;
}