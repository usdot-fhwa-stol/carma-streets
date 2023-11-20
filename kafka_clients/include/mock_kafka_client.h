#pragma once
#include "kafka_client.h"
#include <gmock/gmock.h>
namespace kafka_clients {
     /**
     * @brief Mock kafka client used for unit testing using gmock. For documentation using gmock mocks 
     * (https://google.github.io/googletest/gmock_for_dummies.html).
     * 
     * @author
     */
    class mock_kafka_client : public kafka_client {
        public:
            mock_kafka_client() {};
            ~mock_kafka_client() = default;
            MOCK_METHOD(std::shared_ptr<kafka_clients::kafka_consumer_worker>, create_consumer,(const std::string &broker_str, const std::string &topic_str, const std::string &group_id_str),(const, override));
            MOCK_METHOD(std::shared_ptr<kafka_clients::kafka_producer_worker>, create_producer,(const std::string &broker_str, const std::string &topic_str),(const, override));

    };
}