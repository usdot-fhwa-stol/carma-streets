#pragma once
#include "kafka_client.h"
#include "signal_opt_messages_worker.h"
#include "streets_configuration.h"

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
        std::shared_ptr<std::list<std::pair<int, int>>> _movement_groups;

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
        void start() const;

        void consume_spat() const;

        void consume_vsi() const;

        void consume_tsc_config() const;

        void populate_movement_groups();
        

        /**
         * @brief Updating the intersection info.
         * @param sleep_millisecs The current thread sleep for milliseconds after each update attempt
         * @param int_client_request_attempts The number of attempts for the loop
         * @return boolean. True if intersection information is updated, otherwise failed to update intersection information
         */
        bool update_intersection_info(unsigned long sleep_millisecs, unsigned long int_client_request_attempts) const;
    };
}