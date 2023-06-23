#pragma once

#include <stdexcept>



namespace streets_timing_plan {
    /**
     * @brief Runtime error related to processing streets_timing_plan.
     */ 
    class streets_timing_plan_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~streets_timing_plan_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit streets_timing_plan_exception(const std::string &msg );
    };
}