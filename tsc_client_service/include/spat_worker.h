#pragma once

#include <fstream>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "spat_worker_exception.h"

namespace traffic_signal_controller_service
{
    class spat_worker 
    {
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
             * @brief Construct a new Spat Worker object. This will initialize the member variables to the values assigned in the
             * manifest json file
             * 
             * @param ip The ip address of the tsc_service
             * @param port The ethernet port to receive spat messages on
             * @param socketTimeout Timeout, in seconds, for udp socket to TSC
             */
            spat_worker(const std::string& ip, const int& port, const int& socket_timeout);

            /**
             * @brief Create a UDP socket to the ip and port member variables. If it is successfully created, the received NTCIP SPaT
             * packets and print out their content.
             * 
             * @throw spat_worker_exception if the UDP socket fails to connect or the connection times out. The connection will timout 
             * after a configurable ammount of time if no data is received at UDP socket.
             */
            void listen_udp_spat();    

    };
}