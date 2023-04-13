#include "streets_singleton_exception.h"

namespace streets_service {
    // Constructor
    streets_singleton_exception::streets_singleton_exception(const std::string &msg ): std::runtime_error(msg){};
    // Destructor
    streets_singleton_exception::~streets_singleton_exception() = default;
}