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
#include <sensor_data_sharing_msg/position_confidence.hpp>

using namespace streets_utils::messages::sdsm;

TEST(position_confidence_test, test_from_int){
    EXPECT_EQ(position_confidence::UNAVAILABLE, position_confidence_from_int(0));
    EXPECT_EQ(position_confidence::A_500M, position_confidence_from_int(1));
    EXPECT_EQ(position_confidence::A_200M, position_confidence_from_int(2));
    EXPECT_EQ(position_confidence::A_100M, position_confidence_from_int(3));
    EXPECT_EQ(position_confidence::A_50M, position_confidence_from_int(4));
    EXPECT_EQ(position_confidence::A_20M, position_confidence_from_int(5));
    EXPECT_EQ(position_confidence::A_10M, position_confidence_from_int(6));
    EXPECT_EQ(position_confidence::A_5M, position_confidence_from_int(7));
    EXPECT_EQ(position_confidence::A_2M, position_confidence_from_int(8));
    EXPECT_EQ(position_confidence::A_1M, position_confidence_from_int(9));
    EXPECT_EQ(position_confidence::A_50CM, position_confidence_from_int(10));
    EXPECT_EQ(position_confidence::A_20CM, position_confidence_from_int(11));
    EXPECT_EQ(position_confidence::A_10CM, position_confidence_from_int(12));
    EXPECT_EQ(position_confidence::A_5CM, position_confidence_from_int(13));
    EXPECT_EQ(position_confidence::A_2CM, position_confidence_from_int(14));
    EXPECT_EQ(position_confidence::A_1CM, position_confidence_from_int(15));
    EXPECT_THROW( position_confidence_from_int(16), std::invalid_argument);

}