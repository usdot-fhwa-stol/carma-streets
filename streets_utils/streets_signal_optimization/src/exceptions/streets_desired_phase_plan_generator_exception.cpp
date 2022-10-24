#include "streets_desired_phase_plan_generator_exception.h"

namespace streets_signal_optimization {

    streets_desired_phase_plan_generator_exception::streets_desired_phase_plan_generator_exception(const std::string &msg): std::runtime_error(msg){};

    streets_desired_phase_plan_generator_exception::~streets_desired_phase_plan_generator_exception() = default;
}