#include <gtest/gtest.h>
#include "streets_service.h"
#include "mock_kafka_consumer_worker.h"
#include "mock_kafka_producer_worker.h"

using testing::_;
using testing::Return;

namespace streets_service{

    class test_streets_service : public testing::Test {
        protected:
            void SetUp() {
                setenv("SIMULATION_MODE", "TRUE", 1);
                setenv("TIME_SYNC_TOPIC", "time_sync", 1);
                setenv("CONFIG_FILE_PATH", "../test/test_files/manifest.json", 1);
            }
        public:
            streets_service serv;    
    };

    TEST_F(test_streets_service, test_initialize_sim) {
        ASSERT_TRUE(serv.initialize());
        ASSERT_EQ( serv.get_service_name(), "test_service");
        ASSERT_TRUE(serv.is_simulation_mode());
    };

    TEST_F(test_streets_service, test_consume_time_sync_message) {
        auto mock_time_consumer = std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
        serv._time_consumer = mock_time_consumer;
        EXPECT_CALL(*mock_time_consumer,is_running()).Times(4).WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillRepeatedly(Return(false));
        EXPECT_CALL(*mock_time_consumer, consume(_)).Times(3).WillOnce(Return(""))
                                                            .WillOnce(Return("NOT JSON"))
                                                            .WillOnce(Return(
                                                                "{"
                                                                    "\"timestep\":1400,"
                                                                    "\"seq\":123"
                                                                "}"
                                                            ));

        serv.consume_time_sync_message();   // Skip empty message and skip incorrect message and consume real message then 
                                            // consumer is_running returns false and returns control


        ASSERT_EQ(1400, streets_clock_singleton::time_in_ms());
    }


    TEST_F(test_streets_service, test_initialize_consumer) {
        serv._service_name ="TestService";
        std::shared_ptr<kafka_clients::kafka_consumer_worker> consumer;
        ASSERT_TRUE(serv.initialize_kafka_consumer("test_topic", consumer));
        consumer->stop();
    };

    TEST_F(test_streets_service, test_initialize_producer) {
        serv._service_name ="TestService";
        std::shared_ptr<kafka_clients::kafka_producer_worker> producer;
        ASSERT_TRUE(serv.initialize_kafka_producer("test_topic", producer));
        producer->stop();
    };

    TEST_F(test_streets_service, test_get_system_config) {
        std::string simulation_mode = serv.get_system_config("SIMULATION_MODE");
        ASSERT_EQ(simulation_mode, "TRUE");

        ASSERT_THROW(serv.get_system_config("NON_EXISTANT"), std::runtime_error);
        ASSERT_THROW(serv.get_system_config(nullptr), std::runtime_error);
    };
    TEST_F(test_streets_service, test_start) {
        ASSERT_TRUE(serv.initialize());
        serv.start();
    }

    TEST_F(test_streets_service, test_initialize_exception) {
        unsetenv("CONFIG_FILE_PATH");
        ASSERT_FALSE(serv.initialize());
    }

    TEST_F(test_streets_service, test_initialize_exception_config ) {
        setenv("CONFIG_FILE_PATH", "../test/test_files/invalid.json", 1);
        ASSERT_FALSE(serv.initialize());

    }

}