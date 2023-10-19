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