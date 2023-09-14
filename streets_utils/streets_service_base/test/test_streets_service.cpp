#include <gtest/gtest.h>
#include <streets_service.h>
#include <streets_environment_variables.h>
#include <mock_kafka_consumer_worker.h>
#include <mock_kafka_producer_worker.h>
#include <iostream>
#include <fstream>

using testing::_;
using testing::Return;

namespace streets_service{

    class test_streets_service : public testing::Test {
        protected:
            void SetUp() {
                setenv(SIMULATION_MODE_ENV.c_str(), "TRUE", 1);
                setenv(TIME_SYNC_TOPIC_ENV.c_str(), "time_sync", 1);
                setenv(CONFIG_FILE_PATH_ENV.c_str(), "../test/test_files/manifest.json", 1);
                setenv(LOGS_DIRECTORY_ENV.c_str(), "../logs/", 1);
            }
        public:
            streets_service serv;    
    };

    TEST_F(test_streets_service, test_initialize_sim) {
        EXPECT_TRUE(serv.initialize());
        EXPECT_EQ( serv.get_service_name(), "test_service");
        EXPECT_TRUE(serv.is_simulation_mode());
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


        EXPECT_EQ(1400, streets_clock_singleton::time_in_ms());
    }

    TEST_F(test_streets_service, test_create_daily_logger) {
        serv.initialize();
        auto logger = serv.create_daily_logger("Test_log", ".test", "%v", spdlog::level::critical);
        EXPECT_EQ(spdlog::level::critical, logger->level());
        EXPECT_EQ("Test_log", logger->name());
        std::fstream log_file;
        std::string content;
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        char buffer[128];
        strftime(buffer, sizeof(buffer), "_%Y-%m-%d", now);        
        std::string file_path_string = "../logs/" + logger->name()+ buffer + ".test";
        log_file.open(file_path_string, std::ios::out);
        EXPECT_TRUE(log_file.good());
        log_file.close();
        
    }

    TEST_F(test_streets_service, test_create_daily_logger_default) {
        serv.initialize();
        auto logger = serv.create_daily_logger("default_daily");
        EXPECT_EQ(spdlog::level::info, logger->level());
        EXPECT_EQ("default_daily", logger->name());
        std::fstream log_file;
        std::string content;
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        char buffer[128];
        strftime(buffer, sizeof(buffer), "_%Y-%m-%d", now);        
        std::string file_path_string = "../logs/" + logger->name()+ buffer + ".log";
        log_file.open(file_path_string, std::ios::out);
        EXPECT_TRUE(log_file.good());
        log_file.close();
    }
    
    TEST_F(test_streets_service, test_initialize_consumer) {
        serv._service_name ="TestService";
        std::shared_ptr<kafka_clients::kafka_consumer_worker> consumer;
        EXPECT_TRUE(serv.initialize_kafka_consumer("test_topic", consumer));
        consumer->stop();
    };

    TEST_F(test_streets_service, test_initialize_producer) {
        serv._service_name ="TestService";
        std::shared_ptr<kafka_clients::kafka_producer_worker> producer;
        EXPECT_TRUE(serv.initialize_kafka_producer("test_topic", producer));
        producer->stop();
    };

    TEST_F(test_streets_service, test_get_system_config) {
        std::string simulation_mode = serv.get_system_config("SIMULATION_MODE", "DEFAULT");
        EXPECT_EQ(simulation_mode, "TRUE");

        EXPECT_EQ(serv.get_system_config("NON_EXISTANT", "DEFAULT"), "DEFAULT");
        EXPECT_EQ(serv.get_system_config(nullptr, "DEFAULT"), "DEFAULT");
    };
    TEST_F(test_streets_service, test_start) {
        EXPECT_TRUE(serv.initialize());
        serv.start();
    }

    TEST_F(test_streets_service, test_initialize_defaults) {
        unsetenv(SIMULATION_MODE_ENV.c_str());
        unsetenv(TIME_SYNC_TOPIC_ENV.c_str());
        // Default Config file path does not work here since there is not configuration 
        // file in the package directory.
        // unsetenv(CONFIG_FILE_PATH_ENV.c_str());
        unsetenv(LOGS_DIRECTORY_ENV.c_str());
        EXPECT_TRUE(serv.initialize());
    }

    TEST_F(test_streets_service, test_initialize_exception_config ) {
        setenv("CONFIG_FILE_PATH", "../test/test_files/invalid.json", 1);
        EXPECT_FALSE(serv.initialize());

    }

}