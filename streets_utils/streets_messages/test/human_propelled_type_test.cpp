#include <gtest/gtest.h>
#include <sensor_data_sharing_msg/vru/human_propelled_type.hpp>

using namespace streets_utils::messages;

TEST(human_propelled_type_test, test_from_int){
    EXPECT_EQ(human_propelled_type::UNAVAILABLE, human_propelled_type_from_int(0));
    EXPECT_EQ(human_propelled_type::OTHER_TYPES, human_propelled_type_from_int(1));
    EXPECT_EQ(human_propelled_type::ON_FOOT, human_propelled_type_from_int(2));
    EXPECT_EQ(human_propelled_type::SKATEBOARD, human_propelled_type_from_int(3));
    EXPECT_EQ(human_propelled_type::PUSH_OR_KICK_SCOOTER, human_propelled_type_from_int(4));
    EXPECT_EQ(human_propelled_type::WHEELCHAIR, human_propelled_type_from_int(5));
    // Value outside of range set to unavailable
    EXPECT_EQ(human_propelled_type::UNAVAILABLE, human_propelled_type_from_int(50));

}