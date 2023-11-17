#ifndef KAFKA_CLIENT_H
#define KAFKA_CLIENT_H

#include "kafka_producer_worker.h"
#include "kafka_consumer_worker.h"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <spdlog/spdlog.h>
#include <rapidjson/document.h>


namespace kafka_clients
{

    class kafka_client
    {
    public:
        virtual ~kafka_client()=default;
        virtual std::shared_ptr<kafka_clients::kafka_consumer_worker> create_consumer(const std::string &broker_str, const std::string &topic_str,
                                                                              const std::string &group_id_str) const;
        virtual std::shared_ptr<kafka_clients::kafka_producer_worker> create_producer(const std::string &broker_str, const std::string &topic_str) const;
    };

}

#endif // !KAFKA_CLIENT_H