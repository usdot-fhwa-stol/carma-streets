#include "status_intent_processing_exception.h"

namespace streets_vehicles {

    status_intent_processing_exception::status_intent_processing_exception(const std::string &msg): std::runtime_error(msg){};

    status_intent_processing_exception::~status_intent_processing_exception() = default;
}