#include "streets_timing_plan_exception.h"

namespace streets_timing_plan {

    streets_timing_plan_exception::streets_timing_plan_exception(const std::string &msg): std::runtime_error(msg){};

    streets_timing_plan_exception::~streets_timing_plan_exception() = default;
}