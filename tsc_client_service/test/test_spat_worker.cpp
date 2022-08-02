#include <gtest/gtest.h>
#include <spat_worker.h>

using namespace traffic_signal_controller_service;

TEST(test_spat_worker, test_initialization) {
    spat_worker worker("127.0.0.1", 67921, 2);
    ASSERT_TRUE( worker.initialize());
}