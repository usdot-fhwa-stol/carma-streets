#include "streets_phase_control_schedule_exception.h"

namespace streets_phase_control_schedule {

    streets_phase_control_schedule_exception::streets_phase_control_schedule_exception(const std::string &msg): std::runtime_error(msg){};

    streets_phase_control_schedule_exception::~streets_phase_control_schedule_exception() = default;
}