#pragma once

#include <stdexcept>



namespace streets_vehicles {
    /**
     * @brief Runtime error related to processing status intent vehicle updates.
     * 
     * @author Paul Bourelly 
     */ 
    class status_intent_processing_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~status_intent_processing_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit status_intent_processing_exception(const std::string &msg );
    };
}