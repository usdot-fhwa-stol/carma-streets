#include <gtest/gtest.h>
#include "streets_service.h"
#include "mock_kafka_consumer_worker.h"
#include "mock_kafka_producer_worker.h"

using testing::_;
using testing::Return;

namespace streets_service{

    class test_streets_service : public testing::Test {

    };

    TEST_F(test_streets_service, test_initialize_sim) {
        setenv("SIMULATION_MODE", "TRUE", 1);
        setenv("TIME_SYNC_TOPIC", "time_sync", 1);
        setenv("CONFIG_FILE_PATH", "../test/test_files/manifest.json", 1);
        streets_service serv;
        ASSERT_TRUE(serv.initialize());
    };

    TEST_F(test_streets_service, test_consume_time_sync_message) {
        setenv("SIMULATION_MODE", "TRUE", 1);
        setenv("TIME_SYNC_TOPIC", "time_sync", 1);
        setenv("CONFIG_FILE_PATH", "../test/test_files/manifest.json", 1);
        streets_service serv;
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

        ASSERT_THROW(serv.consume_time_sync_message(), std::runtime_error);;

        serv.consume_time_sync_message();

        ASSERT_EQ(1400, streets_clock_singleton::time_in_ms());

    


    }

}