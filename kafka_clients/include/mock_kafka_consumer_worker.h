#pragma once
#include "kafka_consumer_worker.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class mock_kafka_consumer_worker : public kafka_clients::kafka_consumer_worker {
    public:
        MOCK_METHOD(bool, init,(),(override));
        MOCK_METHOD(const char*, consume, (int timeout_ms), (override));
        MOCK_METHOD(void, subscribe, (), (override));
        MOCK_METHOD(void, stop, (), (override));
        MOCK_METHOD(void, printCurrConf, (), (override));
        MOCK_METHOD(bool, is_running, (), (const override));
        ~mock_kafka_consumer_worker()= default;
}