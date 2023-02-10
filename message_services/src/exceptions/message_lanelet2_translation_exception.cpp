#include "message_lanelet2_translation_exception.h"

namespace message_services {
    namespace exceptions {
        message_lanelet2_translation_exception::message_lanelet2_translation_exception(const std::string &msg) : std::runtime_error(msg){};

        message_lanelet2_translation_exception::~message_lanelet2_translation_exception() = default;
    }
}