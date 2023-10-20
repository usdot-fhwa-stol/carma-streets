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
#include <sensor_data_sharing_msg/vru/animal_propelled_type.hpp>

using namespace streets_utils::messages;

TEST(animal_propelled_type_test, test_from_int){
    EXPECT_EQ(animal_propelled_type::UNAVAILABLE, animal_propelled_type_from_int(0));
    EXPECT_EQ(animal_propelled_type::OTHER_TYPES, animal_propelled_type_from_int(1));
    EXPECT_EQ(animal_propelled_type::ANIMAL_MOUNTED, animal_propelled_type_from_int(2));
    EXPECT_EQ(animal_propelled_type::ANIMAL_DRAWN_CARRIAGE, animal_propelled_type_from_int(3));
    // Value outside of range set to unavailable
    EXPECT_THROW( animal_propelled_type_from_int(4), std::invalid_argument);


}