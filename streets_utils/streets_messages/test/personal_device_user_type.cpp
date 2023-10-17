#include <gtest/gtest.h>
#include <sensor_data_sharing_msg/vru/personal_device_user_type.hpp>

using namespace streets_utils::messages;

TEST(personal_device_user_type_test, test_from_int){
    EXPECT_EQ(personal_device_user_type::UNAVAILABLE, personal_device_user_type_from_int(0));
    EXPECT_EQ(personal_device_user_type::PEDESTRIAN, personal_device_user_type_from_int(1));
    EXPECT_EQ(personal_device_user_type::PEDALCYCLIST, personal_device_user_type_from_int(2));
    EXPECT_EQ(personal_device_user_type::PUBLIC_SAFETY_WORKER, personal_device_user_type_from_int(3));
    EXPECT_EQ(personal_device_user_type::ANIMAL, personal_device_user_type_from_int(4));
    // Value outside of range set to unavailable
    EXPECT_EQ(personal_device_user_type::UNAVAILABLE, personal_device_user_type_from_int(5));

}