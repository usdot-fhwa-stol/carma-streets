#include <gtest/gtest.h>
#include "streets_service.h"


namespace streets_service{

    class test_streets_service : public testing::Test {

    };

    TEST_F(test_streets_service, test_initialize_sim) {
        SPDLOG_INFO("Starting Unit test");
        setenv("SIMULATION_MODE", "TRUE", 1);
        setenv("TIME_SYNC_TOPIC", "time_sync", 1);
        setenv("CONFIG_FILE_PATH", "../test/test_files/manifest.json", 1);
        streets_service serv;
        SPDLOG_INFO("Env is set to {0}",std::getenv("CONFIG_FILE_PATH"));
        ASSERT_TRUE(serv.initialize());
    };
}