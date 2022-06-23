#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <fstream>
#include <spdlog/spdlog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

class SpatWorker 
{
    public:
    /**
     * @brief Construct a new Spat Worker object. This will initialize the member variables to the values assigned in the
     * manifest json file
     * 
     * @param ip The ip address of the TSC
     * @param port The ethernet port to receive spat messages on
     * @param socketTimeout Timeout, in seconds, for udp socket to TSC
     */
        SpatWorker(std::string ip, int port, int socketTimeout); 

        /**
         * @brief Create a socket to the ip and port member variables. If it is successfully created, the received content will
         * be printed to the spd log.
         * 
         */
        void createSocket();

        /**
         * 
         * @return true if socket has successfully been created
         * @return false if socket has not successfully been created
         */
        bool getSocketStatus() const;

        /**
         * @brief ip address of TSC 
         */
        std::string ip_; 
        
        /**
         * @brief ethernet port to receive spat messages on
         */
        int port_; 

        /**
        * @brief timeout for udp socket to TSC
        */
        int socketTimeout_;

        /**
         * @brief boolean to verify whether socket has successfully been created
         */
        bool socketCreated = false; 

};