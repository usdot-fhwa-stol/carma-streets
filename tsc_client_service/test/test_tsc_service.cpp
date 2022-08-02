#include <gtest/gtest.h>
#include <tsc_service.h>

using namespace traffic_signal_controller_service; 

TEST(test_tsc_service, test_initialization) {
    tsc_service service;
    ASSERT_FALSE(service.initialize());
    // Allow for destructor to run before other test cases
    sleep(1);
}