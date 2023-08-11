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
#include "tsc_configuration_state.h"
#include "ntcip_oids.h"
#include "monitor_states_exception.h"
#include "tsc_configuration_state_exception.h"
#include "monitor_desired_phase_plan.h"
#include "monitor_desired_phase_plan_exception.h"
#include "control_tsc_state.h"
#include "control_tsc_state_exception.h"

#include <mutex>  
#include <gtest/gtest_prod.h>  
#include "streets_service.h"
#include "streets_clock_singleton.h"
#include "spat_projection_mode.h"

namespace traffic_signal_controller_service {

    class tsc_service : public streets_service::streets_service {
        private:
            /**
             * @brief Kafka producer for spat JSON
             */
            std::shared_ptr<kafka_clients::kafka_producer_worker> spat_producer;
            /**
             * @brief Kafka producer for tsc_configuration_state JSON
             */
            std::shared_ptr<kafka_clients::kafka_producer_worker> tsc_config_producer;
            /*
             * @brief Kafka consumer for consuming desired phase plan JSON
             */
            std::shared_ptr<kafka_clients::kafka_consumer_worker> desired_phase_plan_consumer;

            /*
             * @brief Kafka consumer for consuming Phase Control Schedule JSON
             */
            std::shared_ptr<kafka_clients::kafka_consumer_worker> phase_control_schedule_consumer;

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
             * @brief Object used to process and store desired phase plan consumed from kafka,
             */
            std::shared_ptr<monitor_desired_phase_plan> monitor_dpp_ptr;
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
             * @brief Point to phase control schedule object which is updated based on the received JSON message from MMITSS Road side processor.
            */
            std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> phase_control_schedule_ptr;
            /**
             * @brief Pointer to tsc_configuration_state object which is traffic signal controller
             * configuration information obtained from the tsc_state worker
             * and broadcast on the carma-streets kafka broker as a JSON message.
             */
            std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config_state_ptr;
            /**
             * @brief Pointer to intersection_client used to query intersection information including
             * name and id from the intersection_model REST API.
             * 
             */
            std::shared_ptr<intersection_client> intersection_client_ptr;

            /**
             * @brief Pointer to the control_tsc_state worker responsible for make snmp HOLD and OMIT calls to follow the desired phase plan.
             * 
             */
            std::shared_ptr<control_tsc_state> control_tsc_state_ptr_;

            // Counter for publishing the tsc_config information. The configuration will be published a hardcoded 10 times
            int tsc_config_producer_counter_ = 0;
            // Default value for number of attempts to send TSC configuration information.
            int tsc_config_send_attempts = 1;

            // desired phase plan information consumed from desire_phase_plan Kafka topic
            spat_projection_mode spat_proj_mode;

            // Queue to store snmp_cmd_structs which are objects used to run snmp HOLD and OMIT commands
            std::queue<streets_snmp_cmd::snmp_cmd_struct> tsc_set_command_queue_;

            // Configurable sleep duration for control_tsc_state thread in milliseconds. This sleep is required to allow some time between checking queue for control commands
            int control_tsc_state_sleep_dur_ = 0;

            // Configurable parameter that is used to enable logging of snmp commands to a log file if set to true. 
            bool enable_snmp_cmd_logging_ = false;

            //Add Friend Test to share private members
            friend class tsc_service_test;
            FRIEND_TEST(tsc_service_test,test_tsc_control);
            FRIEND_TEST(tsc_service_test,test_produce_tsc_config_json_timeout);
            FRIEND_TEST(tsc_service_test,test_init_kafka_consumer_producer);

            /***
             * Configuration parameter to control different interfaces to schedule the traffic signal controller
             * If false, it will use carma-streets internal signal optimization service and its generated desired phase plan to schedule traffic signal controller. 
             * If true, it will use external MRP (MMITSS Roadside Processor, link: https://github.com/mmitss/mmitss-az) and its generated phase control schedule to schedule the traffic sginal controller.
             * **/
            bool use_mmitss_mrp = false;


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
            bool initialize() override;

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
            void start() override;
            /**
             * @brief Method to receive UDP data from traffic signal controller and broadcast spat JSON data to 
             * the carma-streets kafka broker.
             */
            void produce_spat_json() const;

            /**
             * @brief Method to receive traffic signal controller configuration information from the tsc_state and broadcast spat JSON data to 
             * the carma-streets kafka broker.
             */
            void produce_tsc_config_json() const;

            void consume_desired_phase_plan();
            /**
             * @brief Thread callback function to run the phase control schedule Kafka consumer to consume phase control schedule JSON message
            */
            void consume_phase_control_schedule();
            
            /**
             * @brief Method to control phases on the Traffic Signal Controller by sending OMIT and HOLD commands constructed to 
             * follow the desired phase plan. Calls set_tsc_hold_and_omit_forceoff_call
             **/
            void control_tsc_phases();

            /**
             * @brief Method to set HOLD and OMIT, CALL, FORCEOFF on the Traffic signal controller accorording to the desired phase plan/phase control schedule.
             **/
            void set_tsc_hold_and_omit_forceoff_call();

            /**
             * @brief Method to configure spdlog::logger for logging snmp control commands into daily rotating csv file.
            */
            void configure_snmp_cmd_logger() const;

            void configure_veh_ped_call_logger() const;

            /**
             * @brief Method to log spat latency comparing spat time stamp to current time. Calculates and average over 20
             * messages then resets count and spat_processing_time.
             * @param count reference passed in and incremented on each subsequent call.
             * @param spat_processing_time cumulative spat latency from future event project. Calculated by comparing spat
             * time stamp to current time and summing over 20 messages.
             * @param spat_time_stamp the timestamp of the most recent processed spat message.
             */
            void log_spat_latency(int &count, uint64_t &spat_processing_time, uint64_t spat_time_stamp) const;

    };
}