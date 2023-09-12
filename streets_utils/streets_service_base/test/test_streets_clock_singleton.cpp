#include <gtest/gtest.h>
#include "streets_clock_singleton.h"
#include <thread>

using namespace streets_service;
using namespace std::chrono;
namespace streets_service{


   class test_streets_clock : public testing::Test {
        protected:
            void SetUp() {
               SPDLOG_INFO("Setup");
            }
            void TearDown() {
                SPDLOG_INFO("Tear Down");
            }
        
    };

    TEST_F( test_streets_clock, test_simulation_mode) {
        SPDLOG_INFO("Creating simulation clock");
        streets_clock_singleton::create(true);
        // Providing a seed value
        srand((unsigned) time(NULL));
        // Get a random number
        int random = rand();
        int old_val = random;
        spdlog::set_level(spdlog::level::trace);
        //Initialize time at zero to avoid wait_for_initialization hang
        std::thread t1([]() {
            SPDLOG_INFO("Waiting on update 0 ... ");
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            streets_clock_singleton::update(0);
        });
        ASSERT_EQ(streets_clock_singleton::time_in_ms(), 0);
        SPDLOG_INFO("Assertion Successful!");

        
        std::thread t2([random]() {
            SPDLOG_INFO("Waiting on update {0} ... ", random);
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            streets_clock_singleton::update(random);
        });
        streets_clock_singleton::sleep_until(random);
        ASSERT_EQ(streets_clock_singleton::time_in_ms(), random);
        SPDLOG_INFO("Assertion Successful!");

        // Simulate random size timestep
        random += rand();
        streets_clock_singleton::update(random);

        ASSERT_EQ(streets_clock_singleton::time_in_ms(), random);
        ASSERT_NE(old_val, random);
        t1.join();
        t2.join();

    };

    TEST_F(test_streets_clock, test_real_time) {
        streets_clock_singleton::create(false);
        EXPECT_NEAR(streets_clock_singleton::time_in_ms(), duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), 1);
        // allow time to change
        sleep(1);
        EXPECT_NEAR(streets_clock_singleton::time_in_ms(), duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), 1);
        auto cur_time = streets_clock_singleton::time_in_ms();
        streets_clock_singleton::sleep_for(2000);
        EXPECT_NEAR(cur_time+2000, duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), 1);

        cur_time = streets_clock_singleton::time_in_ms();
        auto sleep_until_time = cur_time + 3000;
        streets_clock_singleton::sleep_until(sleep_until_time);
        EXPECT_NEAR(sleep_until_time,  duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count(), 1);

    }
}