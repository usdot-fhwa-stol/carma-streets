#include <gtest/gtest.h>
#include <sensor_data_sharing_msg/vru/motorized_propelled_type.hpp>

using namespace streets_utils::messages;

TEST(motorized_propelled_type_test, test_from_int){
    EXPECT_EQ(motorized_propelled_type::UNAVAILABLE, motorized_propelled_type_from_int(0));
    EXPECT_EQ(motorized_propelled_type::OTHER_TYPES, motorized_propelled_type_from_int(1));
    EXPECT_EQ(motorized_propelled_type::WHEEL_CHAIR, motorized_propelled_type_from_int(2));
    EXPECT_EQ(motorized_propelled_type::BICYCLE, motorized_propelled_type_from_int(3));
    EXPECT_EQ(motorized_propelled_type::SCOOTER, motorized_propelled_type_from_int(4));
    EXPECT_EQ(motorized_propelled_type::SELF_BALANCING_DEVICE, motorized_propelled_type_from_int(5));
    // Value outside of range set to unavailable
    EXPECT_EQ(motorized_propelled_type::UNAVAILABLE, motorized_propelled_type_from_int(7));

}