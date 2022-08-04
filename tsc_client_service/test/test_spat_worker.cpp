#include <gtest/gtest.h>
#include <spat_worker.h>
#include <spat.h>

using namespace traffic_signal_controller_service;

TEST(test_spat_worker, test_receive_timeout) {
    spat_worker worker("127.0.0.1", 67921, 2, false);
    auto spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
    ASSERT_TRUE( worker.initialize());
    try{
            worker.receive_spat(spat_ptr);
    }
    catch( const udp_socket_listener_exception &ex){
        SPDLOG_INFO("Catching correct exception");
        ASSERT_STREQ( ex.what(), "Timeout of 2 seconds has elapsed. Closing SPaT Work UDP Socket");
    }
    catch( ... ) {
        GTEST_FAIL();
    }

}

TEST(test_spat_worker, test_initialization) {
    spat_worker worker("192.168.120.51", 67921, 2, false);
    auto spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
    ASSERT_FALSE( worker.initialize());

}