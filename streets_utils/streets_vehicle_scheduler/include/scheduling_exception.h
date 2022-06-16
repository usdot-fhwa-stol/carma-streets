#pragma once

#include <stdexcept>



namespace streets_vehicle_scheduler {
    /**
     * @brief Runtime error related to processing status intent vehicle updates.
     * 
     * @author Paul Bourelly 
     */ 
    class scheduling_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~scheduling_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit scheduling_exception(const std::string &msg );
    };
}