#pragma once

#include <stdexcept>



namespace traffic_signal_controller_service {
    /**
     * @brief Runtime error related to spat worker and the UDP socket it uses to listen for incoming UDP NTCIP SPaT data.
     * 
     * @author Paul Bourelly 
     */ 
    class spat_worker_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~spat_worker_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit spat_worker_exception(const std::string &msg );
    };
}