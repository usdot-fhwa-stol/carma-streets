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
#include <detected_object_to_sdsm_converter.hpp>
#include <streets_utils/streets_messages_lib/sensor_data_sharing_msg/object_type.hpp>

namespace sensor_data_sharing_service {
    TEST(detected_object_to_sdsm_converter_test, test_to_object_type){
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::VEHICLE, to_object_type("CAR"));
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::VRU, to_object_type("CYCLIST"));
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::UNKNOWN, to_object_type("TREE"));
    }

    TEST(detected_object_to_sdsm_converter_test, test_to_detected_object_data) {
        // Create detected object
        streets_utils::messages::detected_objects_msg::detected_objects_msg msg;
        msg._object_id = "123";
        msg._type = "TREE";
        msg._sensor_id = "sensor_1";
        msg._proj_string = "some string";
        msg._confidence = 0.9;
        msg._velocity = {0.1, 2.3, 5.2};
        msg._angular_velocity = {0.1, 2.3, 5.2};
        msg._position = {0.1, 2.3, 5.2};
        msg._position_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._angular_velocity_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._position_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._size._length =1.1;
        msg._size._width =1.1;
        msg._size._height = 10;

        auto data = to_detected_object_data(msg);
        EXPECT_EQ(std::stoi(msg._object_id), data._detected_object_common_data._object_id);
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::UNKNOWN, data._detected_object_common_data._object_type);
        EXPECT_EQ(90, data._detected_object_common_data._classification_confidence);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._x*10), data._detected_object_common_data._position_offset._offset_x);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._y*10), data._detected_object_common_data._position_offset._offset_y);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._y*10), data._detected_object_common_data._position_offset._offset_y);

        EXPECT_EQ(static_cast<unsigned int>(std::hypot(msg._velocity._x*50, msg._velocity._y*50)), data._detected_object_common_data._speed);
        EXPECT_EQ(static_cast<unsigned int>(msg._velocity._z*50), data._detected_object_common_data._speed_z);
        EXPECT_EQ(static_cast<unsigned int>(msg._size._length*10), std::get<streets_utils::messages::sdsm::detected_obstacle_data>(data._detected_object_optional_data.value())._size._length);


    }

    TEST(detected_object_to_sdsm_converter_test, to_sdsm_timestamp_test) {
        // Time 2023-12-11T19:07:44.075Z
        uint64_t epoch_timestamp = 1702321664075;
        auto sdsm_timestamp = to_sdsm_timestamp(epoch_timestamp);
        EXPECT_EQ(2023, sdsm_timestamp.year);
        EXPECT_EQ(12, sdsm_timestamp.month);
        EXPECT_EQ(11, sdsm_timestamp.day);
        EXPECT_EQ(19, sdsm_timestamp.hour);
        EXPECT_EQ(7, sdsm_timestamp.minute);
        EXPECT_EQ(44075, sdsm_timestamp.second);     
    }
}