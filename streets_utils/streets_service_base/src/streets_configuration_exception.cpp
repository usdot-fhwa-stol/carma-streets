#include "streets_configuration_exception.h"

// Constructor
streets_configuration_exception::streets_configuration_exception(const std::string msg): std::runtime_error(msg){};
streets_configuration_exception::~streets_configuration_exception(){};