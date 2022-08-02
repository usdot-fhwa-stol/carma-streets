#pragma once

#include <spdlog/spdlog.h>
#include <iostream>
#include <string>

#include "ntcip_1202_ext.h"
#include "spat.h"
#include "udp_socket_listener.h"
#include "udp_socket_listener_exception.h"


namespace traffic_signal_controller_service
{
    class spat_worker
    {
        private:
            /**
             * @brief ip address of tsc_service 
             */
            std::string socket_ip_; 
            
            /**
             * @brief ethernet port on tsc_service host to receive spat messages on
             */
            int socket_port_; 

            /**
            * @brief timeout in milliseconds for udp socket to tsc_service
            */
            int socket_timeout_;

            /**
             * @brief boolean to verify whether socket has successfully been created
             */
            bool socket_created_ = false; 

            /**
             * @brief UDP Socket listener to receive UDP NTCIP data packets from Traffic Signal Controller 
             * 
             */
            std::unique_ptr<udp_socket_listener> spat_listener;

        public:
            /**
             * @brief Construct a new Spat Worker object. This will initialize the member variables to the values assigned in the
             * manifest json file
             * 
             * @param ip The ip address of the tsc_service
             * @param port The ethernet port to receive spat messages on
             * @param socketTimeout Timeout, in seconds, for udp socket to TSC
             */
            spat_worker(const std::string& ip, const int& port, const int& socket_timeout);
            
            /**
             * @brief Initilaize UDP Listener socket.
             * 
             * @return true if initialization is successful.
             * @return false if initialization is unsuccessful.
             */
            bool initialize();
            /**
             * @brief Create a UDP socket to the ip and port member variables. If it is successfully created, the received NTCIP SPaT
             * packets and print out their content.
             * 
             * @throw udp_socket_listener_exception if the UDP socket fails to connect or the connection times out. The connection will timout 
             * after a configurable ammount of time if no data is received at UDP socket.
             */
            void receive_spat(const std::shared_ptr<signal_phase_and_timing::spat> _spat_ptr) const ; 

           
    };
}