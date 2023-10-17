#include <gtest/gtest.h>
#include <sensor_data_sharing_msg/equipment_type.hpp>

using namespace streets_utils::messages;

TEST(equipment_type_test, test_from_int){
    EXPECT_EQ(equipment_type::UNKNOWN, equipment_type_from_int(0));
    EXPECT_EQ(equipment_type::RSU, equipment_type_from_int(1));
    EXPECT_EQ(equipment_type::OBU, equipment_type_from_int(2));
    EXPECT_EQ(equipment_type::VRU, equipment_type_from_int(3));
    // Value outside of range set to unavailable
    EXPECT_EQ(equipment_type::UNKNOWN, equipment_type_from_int(5));

}