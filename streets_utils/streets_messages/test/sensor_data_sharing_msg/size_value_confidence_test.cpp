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
#include <sensor_data_sharing_msg/size_value_confidence.hpp>

using namespace streets_utils::messages;

TEST(size_value_confidence_test, test_from_int){
    EXPECT_EQ(size_value_confidence::UNAVAILABLE, size_value_confidence_from_int(0));
    EXPECT_EQ(size_value_confidence::SIZE_100, size_value_confidence_from_int(1));
    EXPECT_EQ(size_value_confidence::SIZE_50, size_value_confidence_from_int(2));
    EXPECT_EQ(size_value_confidence::SIZE_20, size_value_confidence_from_int(3));
    EXPECT_EQ(size_value_confidence::SIZE_10, size_value_confidence_from_int(4));
    EXPECT_EQ(size_value_confidence::SIZE_5, size_value_confidence_from_int(5));
    EXPECT_EQ(size_value_confidence::SIZE_2, size_value_confidence_from_int(6));
    EXPECT_EQ(size_value_confidence::SIZE_1, size_value_confidence_from_int(7));
    EXPECT_EQ(size_value_confidence::SIZE_0_5, size_value_confidence_from_int(8));
    EXPECT_EQ(size_value_confidence::SIZE_0_2, size_value_confidence_from_int(9));
    EXPECT_EQ(size_value_confidence::SIZE_0_1, size_value_confidence_from_int(10));
    EXPECT_EQ(size_value_confidence::SIZE_0_05, size_value_confidence_from_int(11));
    EXPECT_EQ(size_value_confidence::SIZE_0_02, size_value_confidence_from_int(12));
    EXPECT_EQ(size_value_confidence::SIZE_0_01, size_value_confidence_from_int(13));
    EXPECT_THROW( size_value_confidence_from_int(14), std::invalid_argument);

}