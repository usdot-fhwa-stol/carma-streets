#include "scheduling_exception.h"

namespace streets_vehicle_scheduler {

    scheduling_exception::scheduling_exception(const std::string &msg): std::runtime_error(msg){};

    scheduling_exception::~scheduling_exception() = default;
}