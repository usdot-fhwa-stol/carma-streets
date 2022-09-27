#pragma once
#include "kafka_consumer_worker.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
namespace kafka_clients {
    class mock_kafka_consumer_worker : public kafka_clients::kafka_consumer_worker {
        public:
            mock_kafka_consumer_worker(const std::string &broker_str="",
                                        const std::string &topic_str="", 
                                        const std::string &group_id="", 
                                        int64_t cur_offset = 0, 
                                        int32_t partition = 0) : kafka_consumer_worker(broker_str,topic_str, group_id, cur_offset, partition ) 
                                        {}
            MOCK_METHOD(bool, init,(),(override));
            MOCK_METHOD(const char*, consume, (int timeout_ms), (override));
            MOCK_METHOD(void, subscribe, (), (override));
            MOCK_METHOD(void, stop, (), (override));
            MOCK_METHOD(void, printCurrConf, (), (override));
            MOCK_METHOD(bool, is_running, (), (const override));
    };
}