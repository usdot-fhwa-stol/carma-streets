#include <gtest/gtest.h>
#include <messages/sensor_data_sharing_msg/size_value_confidence.hpp>

using namespace streets_utils::messages;

TEST(size_value_confidence_test, test_from_int){
    EXPECT_EQ(size_value_confidence::UNAVAILABLE, size_value_confidence_from_int(0));
    EXPECT_EQ(size_value_confidence::SIZE_100, size_value_confidence_from_int(1));
    EXPECT_EQ(size_value_confidence::SIZE_50, size_value_confidence_from_int(2));
    EXPECT_EQ(size_value_confidence::SIZE_20, size_value_confidence_from_int(3));
    EXPECT_EQ(size_value_confidence::SIZE_10, size_value_confidence_from_int(4));
    EXPECT_EQ(size_value_confidence::SIZE_5, size_value_confidence_from_int(5));
    EXPECT_EQ(size_value_confidence::SIZE_2, size_value_confidence_from_int(6));
    EXPECT_EQ(size_value_confidence::SIZE_1, size_value_confidence_from_int(7));
    EXPECT_EQ(size_value_confidence::SIZE_0_5, size_value_confidence_from_int(8));
    EXPECT_EQ(size_value_confidence::SIZE_0_2, size_value_confidence_from_int(9));
    EXPECT_EQ(size_value_confidence::SIZE_0_1, size_value_confidence_from_int(10));
    EXPECT_EQ(size_value_confidence::SIZE_0_05, size_value_confidence_from_int(11));
    EXPECT_EQ(size_value_confidence::SIZE_0_02, size_value_confidence_from_int(12));
    EXPECT_EQ(size_value_confidence::SIZE_0_01, size_value_confidence_from_int(13));
    // Value outside of range set to unavailable
    EXPECT_EQ(size_value_confidence::UNAVAILABLE, size_value_confidence_from_int(14));

}