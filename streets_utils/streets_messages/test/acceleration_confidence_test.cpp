#include <gtest/gtest.h>
#include <sensor_data_sharing_msg/acceleration_confidence.hpp>

using namespace streets_utils::messages;

TEST(acceleration_confidence_test, test_from_int){
    EXPECT_EQ(acceleration_confidence::UNAVAILABLE, acceleration_confidence_from_int(0));
    EXPECT_EQ(acceleration_confidence::ACCL_100, acceleration_confidence_from_int(1));
    EXPECT_EQ(acceleration_confidence::ACCL_10, acceleration_confidence_from_int(2));
    EXPECT_EQ(acceleration_confidence::ACCL_5, acceleration_confidence_from_int(3));
    EXPECT_EQ(acceleration_confidence::ACCL_1, acceleration_confidence_from_int(4));
    EXPECT_EQ(acceleration_confidence::ACCL_0_1, acceleration_confidence_from_int(5));
    EXPECT_EQ(acceleration_confidence::ACCL_0_05, acceleration_confidence_from_int(6));
    EXPECT_EQ(acceleration_confidence::ACCL_0_01, acceleration_confidence_from_int(7));
    // Value outside of range set to unavailable
    EXPECT_EQ(acceleration_confidence::UNAVAILABLE, acceleration_confidence_from_int(8));

}