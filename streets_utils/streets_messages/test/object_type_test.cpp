#include <gtest/gtest.h>
#include <sensor_data_sharing_msg/object_type.hpp>

using namespace streets_utils::messages;

TEST(object_type_test, test_from_int){
    EXPECT_EQ(object_type::UNKNOWN, object_type_from_int(0));
    EXPECT_EQ(object_type::VEHICLE, object_type_from_int(1));
    EXPECT_EQ(object_type::VRU, object_type_from_int(2));
    EXPECT_EQ(object_type::ANIMAL, object_type_from_int(3));

    // Value outside of range set to unavailable
    EXPECT_EQ(object_type::UNKNOWN, object_type_from_int(50));

}