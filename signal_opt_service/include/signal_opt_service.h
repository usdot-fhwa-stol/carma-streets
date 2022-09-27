#pragma once
#include "kafka_client.h"
#include "signal_opt_messages_worker.h"
#include "streets_configuration.h"
#include "movement_group.h"

namespace signal_opt_service
{
    class signal_opt_service
    {
    private:
        std::shared_ptr<signal_opt_messages_worker> _so_msgs_worker_ptr;
        std::string _bootstrap_server;
        std::string _spat_group_id;
        std::string _spat_topic_name;
        std::string _vsi_group_id;
        std::string _vsi_topic_name;
        std::string _tsc_config_group_id;
        std::string _tsc_config_topic_name;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> _vsi_consumer;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> _spat_consumer;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> _tsc_config_consumer;
        std::shared_ptr<movement_groups> _movement_groups;
        std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;
        std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr;
        std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_configuration_ptr;

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
         * @brief Method to consume SPaT JSON from kafka consumer and update 
         * spat pointer in signal_opt_message_worker. 
         * 
         */
        void consume_spat( const std::shared_ptr<kafka_clients::kafka_consumer_worker> spat_consumer,
                            const std::shared_ptr<signal_phase_and_timing::spat> _spat_ptr ) const;
        /**
         * @brief Method to consume vehicle status and intent JSON from kafka 
         * consumer and update vehicle list pointer in signal_opt_message_worker.
         * 
         */
        void consume_vsi(const std::shared_ptr<kafka_clients::kafka_consumer_worker> vsi_consumer,
                            const std::shared_ptr<streets_vehicles::vehicle_list> _vehicle_list_ptr) const;
        /**
         * @brief Method to consume a single TSC Configuration JSON message 
         * from kafka and update the tsc_config_state pointer in the 
         * signal_opt_message_worker.
         * 
         */
        void consume_tsc_config(const std::shared_ptr<kafka_clients::kafka_consumer_worker> tsc_config_consumer, 
                                const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> _tsc_config_ptr) const;
        /**
         * @brief Method to use the tsc_config_state pointer from the 
         * signal_opt_message_worker to populate movement_groups pointer
         * 
         * @param movement_groups shared pointer to list of movement groups
         */
        void populate_movement_groups(std::shared_ptr<movement_groups> _groups, 
                                    const std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config) ;
        /**
         * @brief Updating the intersection info.
         * @param sleep_millisecs The current thread sleep for milliseconds after each update attempt
         * @param int_client_request_attempts The number of attempts for the loop
         * @return boolean. True if intersection information is updated, otherwise failed to update intersection information
         */
        bool update_intersection_info(unsigned long sleep_millisecs, unsigned long int_client_request_attempts) const;
    };
}