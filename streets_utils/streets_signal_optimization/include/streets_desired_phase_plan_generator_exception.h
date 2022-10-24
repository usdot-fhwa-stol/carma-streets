#pragma once

#include <stdexcept>



namespace streets_signal_optimization {
    /**
     * @brief Runtime error related to generating desired phase plan list.
     * 
     */ 
    class streets_desired_phase_plan_generator_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~streets_desired_phase_plan_generator_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit streets_desired_phase_plan_generator_exception(const std::string &msg );
    };
}