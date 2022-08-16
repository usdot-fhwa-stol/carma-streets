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
#include "monitor_states_exception.h"

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

            // Configurable boolean to enable tsc_state to update incoming spat with future movement events calculated using 
            // traffic signal controller configuration information
            bool use_tsc_state_spat_update_ = true;
            
        public:
            tsc_service() = default;

            ~tsc_service();

            // Remove copy constructor
            tsc_service(const tsc_service &) = delete;
            // Remove copy assignment operator
            tsc_service& operator=(const tsc_service &) = delete;
            
            /**
             * @brief Method to initialize the tsc_service.
             * 
             * @return true if successful.
             * @return false if not successful.
             */
            bool initialize();
            /**
             * @brief Initialize Kafka SPaT producer.
             * 
             * @param bootstap_server for CARMA-Streets Kafka broker.
             * @param spat_producer_topic name of topic to produce to.
             * @return true if initialization is successful.
             * @return false if initialization is not successful.
             */
            bool initialize_kafka_producer( const std::string &bootstap_server, const std::string &spat_producer_topic );

            /**
             * @brief Initialize SNMP Client to make SNMP calls to Traffic Signal Controller.
             * 
             * @param server_ip of Traffic Signal Controller. 
             * @param server_port of Traffic Signal Controller NTCIP Server.
             * @param community of SNMP communication used.
             * @param snmp_version of SNMP communication used.
             * @param timeout timeout micro seconds of SNMP connection
             * @return true if initialization is successful.
             * @return false if initialization is not successful.
             */
            bool initialize_snmp_client( const std::string &server_ip, const int server_port, const std::string &community,
                                        const int snmp_version, const int timeout);
            
            /**
             * @brief Initialize TSC State object which uses an SNMP Client to query initial information
             * from the Traffic Signal Controller including default phase sequence, phase timing information,
             * and phase number to signal group mapping.
             * 
             * @param _snmp_client_ptr SNMP client to use for querying initial information.
             * @return true if initialization is successful.
             * @return false if initialization is not successful.
             */
            bool initialize_tsc_state( const std::shared_ptr<snmp_client> _snmp_client_ptr);
            /**
             * @brief Method to enable spat using the TSC Service SNMP Client. 
             * 
             * @return true if SET operation is successful.
             * @return false if SET operation is not successful.
             */
            bool enable_spat() const;

            /**
             * @brief Initialize SPaT Worker which is responsible for standing up UDP Socket Listener and consuming 
             * Traffic Signal Controller NTCIP UDP SPaT messages and updating the SPaT object using this information.
             * 
             * @param udp_socket_ip IP of localhost where UDP packets will be received.
             * @param udp_socket_port Port of localhost where UDP packets will be received.
             * @param timeout Timeout in seconds for UDP client. UDP socket will throw an exception if no messages
             * are receive for timeount window.
             * @param use_tsc_timestamp bool flag to indicate whether to use TSC timestamp provided in UDP packet.
             * @return true if initialization is successful.
             * @return false if initialization is not successful.
             */
            bool initialize_spat_worker(const std::string &udp_socket_ip, const int udp_socket_port, 
                                        const int timeout, const bool use_tsc_timestamp);
            
            /**
             * @brief Initialize Intersection Client used to query Intersection Model REST API for 
             * Intersection Name and Intersection ID for SPaT message.
             * 
             * @return true if initialization is successful.
             * @return false if initialization is not successful.
             */
            bool initialize_intersection_client();
            /**
             * @brief Initialize SPaT pointer with intersection name, intersection id and phase number to signal
             * group mapping not provided in NTCIP UDP messages from TSC but required for interpretation.
             * 
             * @param intersection_name J2735 Intersection name
             * @param intersection_id J2735 Intersection ID
             * @param phase_number_to_signal_group map of phase numbers (NTCIP) to signal groups provided by TSC State
             */
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