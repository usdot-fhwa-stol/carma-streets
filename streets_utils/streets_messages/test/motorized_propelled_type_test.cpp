// Copyright 2019-2023 Leidos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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