#pragma once

#include <stdexcept>



namespace streets_desired_phase_plan_arbitrator {
    /**
     * @brief Runtime error related to processing desired phase plan arbitrator.
     */ 
    class streets_desired_phase_plan_arbitrator_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~streets_desired_phase_plan_arbitrator_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit streets_desired_phase_plan_arbitrator_exception(const std::string &msg );
    };
}