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
#include <gmock/gmock.h>
#include <spdlog/spdlog.h>
#include <detected_object_enu_to_ned_converter.hpp>
namespace sensor_data_sharing_service {
    TEST(DetectedObjectToENUToNEDConverterTest, TestConversion) {
        streets_utils::messages::detected_objects_msg::detected_objects_msg enu_detection;
        auto pos_x =  4.3;
        auto pos_y = -6.3;
        auto pos_z =  10;
        auto vel_x = -3.2;
        auto vel_y = 0.7;
        auto vel_z = -5.0;

        enu_detection._position._x = pos_x;
        enu_detection._position._y = pos_y;
        enu_detection._position._z = pos_z;
        enu_detection._velocity._x = vel_x;
        enu_detection._velocity._y = vel_y;
        enu_detection._velocity._z = vel_z;

        auto ned_detection = detected_object_enu_to_ned(enu_detection);
        EXPECT_DOUBLE_EQ( ned_detection._position._x, pos_y);
        EXPECT_DOUBLE_EQ( ned_detection._position._y, pos_x);
        EXPECT_DOUBLE_EQ( ned_detection._position._z, -pos_z);

        EXPECT_DOUBLE_EQ( ned_detection._velocity._x, vel_y);
        EXPECT_DOUBLE_EQ( ned_detection._velocity._y, vel_x);
        EXPECT_DOUBLE_EQ( ned_detection._velocity._z, -vel_z);

    }
}