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
#include <sensor_data_sharing_msg/heading_confidence.hpp>

using namespace streets_utils::messages;

TEST(heading_confidence_test, test_from_int){
    EXPECT_EQ(heading_confidence::UNAVAILABLE, heading_confidence_from_int(0));
    EXPECT_EQ(heading_confidence::PREC_10_deg, heading_confidence_from_int(1));
    EXPECT_EQ(heading_confidence::PREC_05_deg, heading_confidence_from_int(2));
    EXPECT_EQ(heading_confidence::PREC_01_deg, heading_confidence_from_int(3));
    EXPECT_EQ(heading_confidence::PREC_0_1_deg, heading_confidence_from_int(4));
    EXPECT_EQ(heading_confidence::PREC_0_05_deg, heading_confidence_from_int(5));
    EXPECT_EQ(heading_confidence::PREC_0_01_deg, heading_confidence_from_int(6));
    EXPECT_EQ(heading_confidence::PREC_0_0125_deg, heading_confidence_from_int(7));
    // Value outside of range set to unavailable
    EXPECT_EQ(heading_confidence::UNAVAILABLE, heading_confidence_from_int(8));

}