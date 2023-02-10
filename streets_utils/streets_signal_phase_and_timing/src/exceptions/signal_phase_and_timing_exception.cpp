#include "signal_phase_and_timing_exception.h"

namespace signal_phase_and_timing {

    signal_phase_and_timing_exception::signal_phase_and_timing_exception(const std::string &msg): std::runtime_error(msg){};

    signal_phase_and_timing_exception::~signal_phase_and_timing_exception() = default;
}