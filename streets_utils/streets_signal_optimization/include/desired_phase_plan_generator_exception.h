#pragma once

#include <stdexcept>



namespace streets_signal_optimization {
    /**
     * @brief Runtime error related to generating desired phase plan list.
     * 
     */ 
    class desired_phase_plan_generator_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~desired_phase_plan_generator_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit desired_phase_plan_generator_exception(const std::string &msg );
    };
}