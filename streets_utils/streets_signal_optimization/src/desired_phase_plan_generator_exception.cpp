#include "desired_phase_plan_generator_exception.h"

namespace streets_signal_optimization {

    desired_phase_plan_generator_exception::desired_phase_plan_generator_exception(const std::string &msg): std::runtime_error(msg){};

    desired_phase_plan_generator_exception::~desired_phase_plan_generator_exception() = default;
}