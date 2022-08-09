#pragma once

#include <stdexcept>



namespace traffic_signal_controller_service {
    /**
     * @brief Runtime error related to spat worker and the UDP socket it uses to listen for incoming UDP NTCIP SPaT data.
     * 
     * @author Paul Bourelly 
     */ 
    class udp_socket_listener_exception : public std::runtime_error{
        public:
            /**
             * @brief Destructor.
             */ 
            ~udp_socket_listener_exception() override;
            /**
             * @brief Constructor. 
             * @param msg String exception message.
             */  
            explicit udp_socket_listener_exception(const std::string &msg );
    };
}