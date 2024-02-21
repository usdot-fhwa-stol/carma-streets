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
#include <sensor_data_sharing_msg/vehicle/vehicle_size_confidence.hpp>
using namespace streets_utils::messages::sdsm;

TEST(vehicleSizeConfidenceTest, defaultConstructor) {
    vehicle_size_confidence v_size_conf;
    EXPECT_EQ(v_size_conf._width_confidence, size_value_confidence::UNAVAILABLE);
    EXPECT_EQ(v_size_conf._length_confidence, size_value_confidence::UNAVAILABLE);
    EXPECT_FALSE(v_size_conf._height_confidence.has_value());
    
}