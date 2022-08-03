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
            
            bool initialize_kafka_producer( const std::string &bootstap_server, const std::string &spat_producer_topic );

            bool initialize_snmp_client( const std::string &server_ip, const int server_port, const std::string &community,
                                        const int snmp_version, const int timeout);

            bool initialize_tsc_state( const std::shared_ptr<snmp_client> _snmp_client_ptr);

            bool enable_spat() const;

            bool initialize_spat_worker(const std::string &udp_socket_ip, const int udp_socket_port, 
                                        const int timeout, const bool use_tsc_timestamp);
            
            bool initialize_intersection_client();

            void initialize_spat( const std::string &intersection_name, const int intersection_id, 
                                const std::unordered_map<int,int> &phase_number_to_signal_group);
                    

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