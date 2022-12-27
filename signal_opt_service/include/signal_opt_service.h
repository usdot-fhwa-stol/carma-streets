#pragma once
#include "kafka_client.h"
#include "signal_opt_messages_worker.h"
#include "signal_opt_processing_worker.h"
#include "streets_configuration.h"
#include "movement_group.h"
#include "streets_desired_phase_plan_generator.h"
#include "streets_desired_phase_plan_arbitrator.h"
#include <gtest/gtest_prod.h>

namespace signal_opt_service
{
    class signal_opt_service
    {
    private:
        std::shared_ptr<signal_opt_messages_worker> _so_msgs_worker_ptr;
        std::shared_ptr<signal_opt_processing_worker> _so_processing_worker_ptr;

        std::string _bootstrap_server;
        std::string _spat_group_id;
        std::string _spat_topic_name;
        std::string _vsi_group_id;
        std::string _vsi_topic_name;
        std::string _tsc_config_group_id;
        std::string _tsc_config_topic_name;
        std::string _dpp_topic_name;
        std::string _so_log_path;
        std::string _so_log_filename;
        int _exp_delta;
        bool enable_so_logging = false;
        int so_sleep_time;
        uint64_t _time_to_yellow;
        std::vector<uint> ignore_signal_groups;

        std::shared_ptr<kafka_clients::kafka_consumer_worker> _vsi_consumer;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> _spat_consumer;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> _tsc_config_consumer;
        std::shared_ptr<kafka_clients::kafka_producer_worker> _dpp_producer;
        
        streets_signal_optimization::streets_desired_phase_plan_generator_configuration dpp_config;
        std::shared_ptr<streets_signal_optimization::movement_groups> movement_groups_ptr;
        std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;
        std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr;
        std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_configuration_ptr;


        FRIEND_TEST(signal_opt_service, test_produce_dpp);
        FRIEND_TEST(signal_opt_service, test_read_configuration_params);

    public:
        /**
         * @brief Initialize the consumer and producer configurations
         */
        signal_opt_service() = default;
        /**
         * @brief stop the consumer and producer
         */
        ~signal_opt_service();
        /**
         * @brief Initialize the consumer and producer configurations
         */
        bool initialize();
        /**
         * @brief Create threads and consume messages
         */
        void start();
        /**
         * @brief Method to consume SPaT JSON from kafka consumer and update spat pointer.
         * 
         * @param spat_consumer shared pointer to kafka consumer.
         * @param _spat_ptr shared pointer to spat object.
         */
        void consume_spat( const std::shared_ptr<kafka_clients::kafka_consumer_worker> spat_consumer,
                            const std::shared_ptr<signal_phase_and_timing::spat> _spat_ptr ) const;
        /**
         * @brief Method to consume vehicle status and intent JSON from kafka 
         * consumer and vehicle list pointer.
         * 
         * @param vsi_consumer shared pointer to kafka consumer.
         * @param _vehicle_list_ptr shared pointer to vehicle list object.
         */
        void consume_vsi(const std::shared_ptr<kafka_clients::kafka_consumer_worker> vsi_consumer,
                            const std::shared_ptr<streets_vehicles::vehicle_list> _vehicle_list_ptr) const;
        /**
         * @brief Method to consume a single TSC Configuration JSON message 
         * from kafka and update the tsc configuration state pointer.
         * 
         * @param tsc_config_consumer shared pointer to kafka consumer.
         * @param _tsc_config_ptr shared pointer to tsc configuration state pointer.
         */
        void consume_tsc_config(const std::shared_ptr<kafka_clients::kafka_consumer_worker> tsc_config_consumer, 
                                const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> _tsc_config_ptr) const;
        /**
         * @brief Method to produce desired phase plan Json to kafka.
         * 
         * @param dpp_producer shared pointer to kafka producer.
         * @param _intersection_info_ptr shared pointer to intersection info object.
         * @param _vehicle_list_ptr shared pointer to vehicle list object.
         * @param _spat_ptr shared pointer to spat object.
         * @param _tsc_config_ptr shared pointer to tsc configuration state object.
         * @param _movement_groups_ptr shared pointer to movement groups object.
         * @param _dpp_config desired phase plan generator configuration object.
         * @param _so_sleep_time the time interval between two iterations of calling signal optimization libraries
         */
        void produce_dpp(const std::shared_ptr<kafka_clients::kafka_producer_worker> dpp_producer,
                               const std::shared_ptr<OpenAPI::OAIIntersection_info> _intersection_info_ptr, 
                               const std::shared_ptr<streets_vehicles::vehicle_list> _vehicle_list_ptr, 
                               const std::shared_ptr<signal_phase_and_timing::spat> _spat_ptr, 
                               const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> _tsc_config_ptr, 
                               const std::shared_ptr<streets_signal_optimization::movement_groups> _movement_groups_ptr,
                               const streets_signal_optimization::streets_desired_phase_plan_generator_configuration &_dpp_config,
                               const int &_so_sleep_time) const;
        /**
         * @brief Method to use the tsc_config_state pointer from the 
         * signal_opt_message_worker to populate movement_groups pointer
         * 
         * @param movement_groups shared pointer to list of movement groups
         * @param tsc_config shared pointer to tsc configuration state object.
         */
        void populate_movement_groups(std::shared_ptr<streets_signal_optimization::movement_groups> _groups, 
                                    const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config) const ;
        /**
         * @brief Updating the intersection info.
         * @param sleep_millisecs The current thread sleep for milliseconds after each update attempt
         * @param int_client_request_attempts The number of attempts for the loop
         * @return boolean. True if intersection information is updated, otherwise failed to update intersection information
         */
        bool update_intersection_info(unsigned long sleep_millisecs, unsigned long int_client_request_attempts) const;

        /**
         * @brief Method to configure spdlog::logger for logging signal optimization metrics into daily rotating csv file.
         */
        void configure_csv_logger() const;

        /**
         * @brief Method for reading the configuration parameters from manifest.json.
        */
        void read_configuration_params();

        /**
         * @brief Remove list of provided signal groups from movement groups considered for signal optmization
         * 
         * @param _movement_groups collection of movement groups to remove signal groups from
         * @param rm_signal_groups signal groups to remove from movement groups.
         */
        void remove_signal_groups( const std::shared_ptr<streets_signal_optimization::movement_groups> &movement_groups,
                                     const std::vector<uint> &rm_signal_groups ) const;
    };
}