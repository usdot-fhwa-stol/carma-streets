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

namespace traffic_signal_controller_service
{
    class SpatWorker 
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
            int socketTimeout_;

            /**
             * @brief boolean to verify whether socket has successfully been created
             */
            bool socketCreated_ = false; 

        public:
            /**
             * @brief Construct a new Spat Worker object. This will initialize the member variables to the values assigned in the
             * manifest json file
             * 
             * @param ip The ip address of the tsc_service
             * @param port The ethernet port to receive spat messages on
             * @param socketTimeout Timeout, in seconds, for udp socket to TSC
             */
            SpatWorker(const std::string& ip, const int& port, const int& socketTimeout);

            /**
             * @brief Create a socket to the ip and port member variables. If it is successfully created, the received content will
             * be printed to the spd log.
             * 
             */
            void createSocket();    

    };
}