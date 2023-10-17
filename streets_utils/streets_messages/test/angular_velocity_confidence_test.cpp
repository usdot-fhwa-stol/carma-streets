#include <gtest/gtest.h>
#include <sensor_data_sharing_msg/angular_velocity_confidence.hpp>

using namespace streets_utils::messages;

TEST(angular_velocity_confidence_test, test_from_int){
    EXPECT_EQ(angular_velocity_confidence::UNAVAILABLE, angular_velocity_confidence_from_int(0));
    EXPECT_EQ(angular_velocity_confidence::DEGSEC_100, angular_velocity_confidence_from_int(1));
    EXPECT_EQ(angular_velocity_confidence::DEGSEC_10, angular_velocity_confidence_from_int(2));
    EXPECT_EQ(angular_velocity_confidence::DEGSEC_05, angular_velocity_confidence_from_int(3));
    EXPECT_EQ(angular_velocity_confidence::DEGSEC_01, angular_velocity_confidence_from_int(4));
    EXPECT_EQ(angular_velocity_confidence::DEGSEC_0_1, angular_velocity_confidence_from_int(5));
    EXPECT_EQ(angular_velocity_confidence::DEGSEC_0_05, angular_velocity_confidence_from_int(6));
    EXPECT_EQ(angular_velocity_confidence::DEGSEC_0_01, angular_velocity_confidence_from_int(7));
    // Value outside of range set to unavailable
    EXPECT_EQ(angular_velocity_confidence::UNAVAILABLE, angular_velocity_confidence_from_int(50));

}