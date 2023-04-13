#pragma once

#include <stdexcept>

namespace streets_service {
    /**
     * @brief Streets Singleton Exception thrown when user attempts to retreive
     * singleton before calling create to initialize it.
     */ 
    class streets_singleton_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~streets_singleton_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit streets_singleton_exception(const std::string &msg );
    };
}