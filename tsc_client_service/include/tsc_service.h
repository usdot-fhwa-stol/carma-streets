#pragma once 

#include "kafka_client.h"
#include "spat_worker.h"
#include "streets_configuration.h"
#include "streets_configuration_exception.h"
#include "monitor_tsc_state.h"
#include "snmp_client.h"
#include "spat.h"
#include "udp_socket_listener.h"
#include "intersection_client.h"
#include "ntcip_oids.h"

namespace traffic_signal_controller_service {

    class tsc_service {
        private:
            /**
             * @brief kafka broker host
             */
            std::string _bootstrap_server;
            /**
             * @brief Kafka spat topic group id
             */
            std::string _spat_group_id;
            /**
             * @brief kafka spat topic name
             */
            std::string _spat_topic_name;
            /**
             * @brief Kafka producer for spat JSON
             */
            std::shared_ptr<kafka_clients::kafka_producer_worker> spat_producer;
            /**
             * @brief spat_worker contains udp_socket_listener and consumes UDP data 
             * packets and updates spat accordingly.
             */
            std::shared_ptr<spat_worker> spat_worker_ptr;
            /**
             * @brief tsc_state queries and stores traffic signal controller configurations
             * including phase sequence, signal group (J2735) to phase number (NTCIP) maping
             * and red clearance, yellow change, min/max green times.
             */
            std::shared_ptr<tsc_state> tsc_state_ptr;
            /**
             * @brief snmp_client used for making SNMP GET and SET calls th NTCIP OIDs to set
             * and retrieve information from the traffic signal controller.
             */
            std::shared_ptr<snmp_client> snmp_client_ptr;
            /**
             * @brief Pointer to spat object which is updated based on received UDP messsages from
             * the traffic signal controller and broadcast on the carma-streets kafka broker as a 
             * JSON message.
             */
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;
            /**
             * @brief Pointer to intersection_client used to query intersection information including
             * name and id from the intersection_model REST API.
             * 
             */
            std::shared_ptr<intersection_client> intersection_client_ptr;
        public:
            tsc_service() = default;

            ~tsc_service();
            /**
             * @brief Method to initialize the tsc_service.
             * 
             * @return true if successful.
             * @return false if not successful.
             */
            bool initialize();
            /**
             * @brief Method to start all threads included in the tsc_service.
             */
            void start() ;
            /**
             * @brief Method to receive UDP data from traffic signal controller and broadcast spat JSON data to 
             * the carma-streets kafka broker.
             */
            void produce_spat_json() const;

    };
}