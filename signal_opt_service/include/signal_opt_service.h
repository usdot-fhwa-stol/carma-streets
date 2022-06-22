#pragma once
#include "kafka_client.h"
#include "signal_opt_messages_worker.h"
#include "streets_configuration.h"

namespace signal_opt_service
{
    enum class CONSUME_MSG_TYPE
    {
        SPAT,
        VEHICLE_STATUS_INTENT,
    };

    class signal_opt_service
    {
    private:
        std::shared_ptr<signal_opt_messages_worker> so_msgs_worker_ptr;
        std::string bootstrap_server;
        std::string spat_group_id;
        std::string spat_topic_name;
        std::string vsi_group_id;
        std::string vsi_topic_name;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> _vsi_consumer;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> _spat_consumer;

    public:
        /**
         * @brief Initialize the consumer and producer configurations
         */
        signal_opt_service();
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
         * @brief Consume the different types of kafka stream via kafka consumers
         * @param msg_json The consumed kafka stream in JSON format
         * @param consume_msg_type The type of message consumed by the kafka consumer
         */
        void consume_msg(std::shared_ptr<kafka_clients::kafka_consumer_worker> consumer, CONSUME_MSG_TYPE consume_msg_type);
        /**
         * @brief Updating the intersection info.
         * @param sleep_millisecs The current thread sleep for milliseconds after each update attempt
         * @param int_client_request_attempts The number of attempts for the loop
         * @return boolean. True if intersection information is updated, otherwise failed to update intersection information
         */
        bool update_intersection_info(unsigned long sleep_millisecs, unsigned long int_client_request_attempts);
    };
}