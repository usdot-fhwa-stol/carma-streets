#include <gtest/gtest.h>
#include <messages/sensor_data_sharing_msg/vru/animal_propelled_type.hpp>

using namespace streets_utils::messages;

TEST(animal_propelled_type_test, test_from_int){
    EXPECT_EQ(animal_propelled_type::UNAVAILABLE, animal_propelled_type_from_int(0));
    EXPECT_EQ(animal_propelled_type::OTHER_TYPES, animal_propelled_type_from_int(1));
    EXPECT_EQ(animal_propelled_type::ANIMAL_MOUNTED, animal_propelled_type_from_int(2));
    EXPECT_EQ(animal_propelled_type::ANIMAL_DRAWN_CARRIAGE, animal_propelled_type_from_int(3));
    // Value outside of range set to unavailable
    EXPECT_EQ(animal_propelled_type::UNAVAILABLE, animal_propelled_type_from_int(4));

}