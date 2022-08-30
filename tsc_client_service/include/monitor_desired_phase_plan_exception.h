#pragma once

#include <stdexcept>



namespace traffic_signal_controller_service {
    /**
     * @brief Runtime error related to consuming desired phase plan 
     * 
     */ 
    class monitor_desired_phase_plan_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~monitor_desired_phase_plan_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit monitor_desired_phase_plan_exception(const std::string &msg );
    };
}