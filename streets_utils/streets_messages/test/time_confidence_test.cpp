#include <gtest/gtest.h>
#include <messages/sensor_data_sharing_msg/time_confidence.hpp>

using namespace streets_utils::messages;

TEST(time_confidence_test, test_from_int){
    EXPECT_EQ(time_confidence::UNAVAILABLE, time_confidence_from_int(0));
    EXPECT_EQ(time_confidence::TIME_100_000, time_confidence_from_int(1));
    EXPECT_EQ(time_confidence::TIME_050_000, time_confidence_from_int(2));
    EXPECT_EQ(time_confidence::TIME_020_000, time_confidence_from_int(3));
    EXPECT_EQ(time_confidence::TIME_010_000, time_confidence_from_int(4));
    EXPECT_EQ(time_confidence::TIME_002_000, time_confidence_from_int(5));
    EXPECT_EQ(time_confidence::TIME_001_000, time_confidence_from_int(6));
    EXPECT_EQ(time_confidence::TIME_000_500, time_confidence_from_int(7));
    EXPECT_EQ(time_confidence::TIME_000_200, time_confidence_from_int(8));
    EXPECT_EQ(time_confidence::TIME_000_100, time_confidence_from_int(9));
    EXPECT_EQ(time_confidence::TIME_000_050, time_confidence_from_int(10));
    EXPECT_EQ(time_confidence::TIME_000_020, time_confidence_from_int(11));
    EXPECT_EQ(time_confidence::TIME_000_010, time_confidence_from_int(12));
    EXPECT_EQ(time_confidence::TIME_000_005, time_confidence_from_int(13));
    EXPECT_EQ(time_confidence::TIME_000_002, time_confidence_from_int(14));
    EXPECT_EQ(time_confidence::TIME_000_001, time_confidence_from_int(15));
    EXPECT_EQ(time_confidence::TIME_000_000_5, time_confidence_from_int(16));
    EXPECT_EQ(time_confidence::TIME_000_000_2, time_confidence_from_int(17));
    EXPECT_EQ(time_confidence::TIME_000_000_1, time_confidence_from_int(18));
    EXPECT_EQ(time_confidence::TIME_000_000_05, time_confidence_from_int(19));
    EXPECT_EQ(time_confidence::TIME_000_000_02, time_confidence_from_int(20));
    EXPECT_EQ(time_confidence::TIME_000_000_01, time_confidence_from_int(21));
    EXPECT_EQ(time_confidence::TIME_000_000_005, time_confidence_from_int(22));
    EXPECT_EQ(time_confidence::TIME_000_000_002, time_confidence_from_int(23));
    EXPECT_EQ(time_confidence::TIME_000_000_001, time_confidence_from_int(24));
    EXPECT_EQ(time_confidence::TIME_000_000_000_5, time_confidence_from_int(25));
    EXPECT_EQ(time_confidence::TIME_000_000_000_2, time_confidence_from_int(26));
    EXPECT_EQ(time_confidence::TIME_000_000_000_1, time_confidence_from_int(27));
    EXPECT_EQ(time_confidence::TIME_000_000_000_05, time_confidence_from_int(28));
    EXPECT_EQ(time_confidence::TIME_000_000_000_02, time_confidence_from_int(29));
    EXPECT_EQ(time_confidence::TIME_000_000_000_01, time_confidence_from_int(30));
    EXPECT_EQ(time_confidence::TIME_000_000_000_005, time_confidence_from_int(31));
    EXPECT_EQ(time_confidence::TIME_000_000_000_002, time_confidence_from_int(32));
    EXPECT_EQ(time_confidence::TIME_000_000_000_001, time_confidence_from_int(33));
    EXPECT_EQ(time_confidence::TIME_000_000_000_000_5, time_confidence_from_int(34));
    EXPECT_EQ(time_confidence::TIME_000_000_000_000_2, time_confidence_from_int(35));
    EXPECT_EQ(time_confidence::TIME_000_000_000_000_1, time_confidence_from_int(36));
    EXPECT_EQ(time_confidence::TIME_000_000_000_000_05, time_confidence_from_int(37));
    EXPECT_EQ(time_confidence::TIME_000_000_000_000_02, time_confidence_from_int(38)); 
    EXPECT_EQ(time_confidence::TIME_000_000_000_000_01, time_confidence_from_int(39));
    // Outside valid values defaults to unavailable    
    EXPECT_EQ(time_confidence::UNAVAILABLE, time_confidence_from_int(145));

}