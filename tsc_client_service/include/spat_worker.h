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
#include <QTimer>
#include <QEventLoop>

#include "ntcip_1202_ext.h"
#include "spat_worker_exception.h"
#include "spat.h"
#include "udp_socket_listener.h"
#include "intersection_client_api_lib/OAIDefaultApi.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"

namespace traffic_signal_controller_service
{
    class spat_worker: public QObject
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

            std::string intersection_name_;

            int intersection_id_;

            std::unordered_map<int, int> phase_number_to_signal_group_;

            /**
             * @brief Shared ptr to spat object.
             * 
             */
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;

            upd_socket_listener spat_listener;

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

            bool initialize();
            /**
             * @brief Create a UDP socket to the ip and port member variables. If it is successfully created, the received NTCIP SPaT
             * packets and print out their content.
             * 
             * @throw spat_worker_exception if the UDP socket fails to connect or the connection times out. The connection will timout 
             * after a configurable ammount of time if no data is received at UDP socket.
             */
            void listen_udp_spat(); 

            bool request_intersection_info();

    };
}