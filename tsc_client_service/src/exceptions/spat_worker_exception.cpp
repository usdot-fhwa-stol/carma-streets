#include "spat_worker_exception.h"

namespace traffic_signal_controller_service {

    spat_worker_exception::spat_worker_exception(const std::string &msg): std::runtime_error(msg){};

    spat_worker_exception::~spat_worker_exception() = default;
}