#pragma once

#include <rapidjson/document.h>
#include <string>
#include <mutex>
#include <thread>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include "kafka_client.h"
#include "map.h"

namespace intersection_model
{
    class map_msg_worker
    {
    private:
        // Kafka consumer configuration
        std::string bootstrap_server;
        std::string map_msg_group_id;
        std::string map_msg_topic_name;
        std::shared_ptr<kafka_clients::kafka_consumer_worker> _map_msg_consumer;
        std::mutex intersection_map_msg_mtx;
        std::shared_ptr<intersection_map> _intersection_map_ptr;

    public:
        map_msg_worker() = default;
        explicit map_msg_worker(const int intersection_id);
        map_msg_worker(const int intersection_id, const std::string &bootstrap_server, const std::string &map_msg_group_id, const std::string &map_msg_topic_name);
        ~map_msg_worker() = default;

        /**
         * @brief Update intesection map structure. It will only update when both kafka consumer consumes MAP message and intersection id from MAP message matches id from MAP message
         * @param payload from consumed MAP message 
         * @return true if intersection map is updated
         * @return false if intersection map is not updated
         */
        bool update_map_msg(const std::string &payload);
        
        /**
         * @brief Get the const map msg ptr object and prevent external class from modifying it
         * 
         * @return const std::shared_ptr<intersection_map> 
         */
        const std::shared_ptr<intersection_map> get_map_msg_ptr() const;
        
        /**
         * @brief Get the map msg consumer ptr object and prevent external class from modifying it
         * 
         * @return const std::shared_ptr<kafka_clients::kafka_consumer_worker> 
         */
        const std::shared_ptr<kafka_clients::kafka_consumer_worker> get_map_msg_consumer_ptr() const;

        /**
         * @brief parse MAP msg json string with rapidjson document
         */
        rapidjson::Document parse_JSON_string(const std::string &jsonString);
    };
}
