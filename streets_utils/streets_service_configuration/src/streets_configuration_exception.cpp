#include "streets_configuration_exception.h"

namespace streets_service {
    // Constructor
    streets_configuration_exception::streets_configuration_exception(const std::string &msg ): std::runtime_error(msg){};
    // Destructor
    streets_configuration_exception::~streets_configuration_exception() = default;
}