#pragma once

#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>


namespace traffic_signal_controller_service
{
    class upd_socket_listener{
        private:
            /**
             * @brief ip address of tsc_service 
             */
            std::string ip_; 
            
            /**
             * @brief ethernet port on tsc_service host to receive spat messages on
             */
            int port_; 

            /**
            * @brief timeout in milliseconds for udp socket to tsc_service
            */
            int socket_timeout_;

            /**
             * @brief boolean to verify whether socket has successfully been created
             */
            bool socket_created_ = false; 


        public:
            /**
             * @brief Construct a new udp_socket_listener object. This will initialize the member variables to the values assigned in the
             * manifest json file
             * 
             * @param ip The ip address of the tsc_service
             * @param port The ethernet port to receive spat messages on
             * @param socketTimeout Timeout, in seconds, for udp socket to TSC
             */
            upd_socket_listener(const std::string& ip, const int& port, const int& socket_timeout );

            bool initialize();

            std::vector<char> receive();
    };

    
} // namespace traffic_signal_controller_service
