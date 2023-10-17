#include <gtest/gtest.h>
#include <messages/sensor_data_sharing_msg/position_confidence.hpp>

using namespace streets_utils::messages;

TEST(position_confidence_test, test_from_int){
    EXPECT_EQ(position_confidence::UNAVAILABLE, position_confidence_from_int(0));
    EXPECT_EQ(position_confidence::A_500M, position_confidence_from_int(1));
    EXPECT_EQ(position_confidence::A_200M, position_confidence_from_int(2));
    EXPECT_EQ(position_confidence::A_100M, position_confidence_from_int(3));
    EXPECT_EQ(position_confidence::A_50M, position_confidence_from_int(4));
    EXPECT_EQ(position_confidence::A_20M, position_confidence_from_int(5));
    EXPECT_EQ(position_confidence::A_10M, position_confidence_from_int(6));
    EXPECT_EQ(position_confidence::A_5M, position_confidence_from_int(7));
    EXPECT_EQ(position_confidence::A_2M, position_confidence_from_int(8));
    EXPECT_EQ(position_confidence::A_1M, position_confidence_from_int(9));
    EXPECT_EQ(position_confidence::A_50CM, position_confidence_from_int(10));
    EXPECT_EQ(position_confidence::A_20CM, position_confidence_from_int(11));
    EXPECT_EQ(position_confidence::A_10CM, position_confidence_from_int(12));
    EXPECT_EQ(position_confidence::A_5CM, position_confidence_from_int(13));
    EXPECT_EQ(position_confidence::A_2CM, position_confidence_from_int(14));
    EXPECT_EQ(position_confidence::A_1CM, position_confidence_from_int(15));
    // Value outside of range set to unavailable
    EXPECT_EQ(position_confidence::UNAVAILABLE, position_confidence_from_int(50));

}