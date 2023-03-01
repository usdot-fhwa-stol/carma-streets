#pragma once
#include <stdexcept>

namespace message_services
{
    namespace exceptions
    {
        /**
         * @brief Runtime error related to lanelet2 translation for message location values.
         * 
         * @author Paul Bourelly 
         */ 
        class message_lanelet2_translation_exception : public std::runtime_error{
            public:
                /**
                 * @brief Destructor.
                 */ 
                ~message_lanelet2_translation_exception() override;
                /**
                 * @brief Constructor. 
                 * @param msg String exception message.
                 */  
                explicit message_lanelet2_translation_exception(const std::string &msg );
        };
    }
}