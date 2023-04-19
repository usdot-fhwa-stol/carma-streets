#include <gtest/gtest.h>
#include "streets_clock_singleton.h"

using namespace streets_service;
using namespace std::chrono;

TEST( test_streets_clock_singleton, test_simulation_mode) {
    streets_clock_singleton::create(true);
    // Providing a seed value
	srand((unsigned) time(NULL));
	// Get a random number
	int random = rand();
    int old_val = random;
    //Initialize time at zero to avoid wait_for_initialization hang
    streets_clock_singleton::update(0);

    ASSERT_EQ(streets_clock_singleton::time_in_ms(), 0);

    streets_clock_singleton::update(random);

    ASSERT_EQ(streets_clock_singleton::time_in_ms(), random);
    // Simulate random size timestep
    random += rand();
    streets_clock_singleton::update(random);

    ASSERT_EQ(streets_clock_singleton::time_in_ms(), random);
    ASSERT_NE(old_val, random);

};

TEST(test_streets_clock_singleton, test_real_time) {
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