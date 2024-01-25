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
    TEST(detectedObjectToSdsmConverterTest, testToObjectType){
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::VEHICLE, to_object_type("CAR"));
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::VRU, to_object_type("CYCLIST"));
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::UNKNOWN, to_object_type("TREE"));
    }

    TEST(detectedObjectToSdsmConverterTest, testToDetectedObjectData) {
        // Create detected object
        streets_utils::messages::detected_objects_msg::detected_objects_msg msg;
        msg._object_id = 123;
        msg._type = "TREE";
        msg._sensor_id = "sensor_1";
        msg._proj_string = "some string";
        msg._confidence = 0.9;
        msg._velocity = {0.1, 2.3, 5.2};
        msg._angular_velocity = {0.1, 2.3, 5.2};
        msg._position = {0.1, 2.3, 5.2};
        msg._position_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._velocity_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._angular_velocity_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._size._length =1.1;
        msg._size._width =1.1;
        msg._size._height = 10;

        auto data = to_detected_object_data(msg);
        EXPECT_EQ(msg._object_id, data._detected_object_common_data._object_id);
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::UNKNOWN, data._detected_object_common_data._object_type);
        EXPECT_EQ(90, data._detected_object_common_data._classification_confidence);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._x*10), data._detected_object_common_data._position_offset._offset_x);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._y*10), data._detected_object_common_data._position_offset._offset_y);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._y*10), data._detected_object_common_data._position_offset._offset_y);

        EXPECT_EQ(115, data._detected_object_common_data._speed);
        EXPECT_EQ(260, data._detected_object_common_data._speed_z);
        EXPECT_EQ(static_cast<unsigned int>(msg._size._length*10), std::get<streets_utils::messages::sdsm::detected_obstacle_data>(data._detected_object_optional_data.value())._size._length);


    }

     TEST(detectedObjectToSdsmConverterTest, testToDetectedObjectDataWithNegativeVelocity) {
        // Create detected object
        streets_utils::messages::detected_objects_msg::detected_objects_msg msg;
        msg._object_id = 123;
        msg._type = "TREE";
        msg._sensor_id = "sensor_1";
        msg._proj_string = "some string";
        msg._confidence = 0.9;
        msg._velocity = {-0.1, -2.3, -5.2};
        msg._angular_velocity = {0.1, 2.3, 5.2};
        msg._position = {0.1, 2.3, 5.2};
        msg._position_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._velocity_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._angular_velocity_covariance = {{0.1, 2.3, 5.2},{0.1, 2.3, 5.2},{0.1, 2.3, 5.2}};
        msg._size._length =1.1;
        msg._size._width =1.1;
        msg._size._height = 10;

        auto data = to_detected_object_data(msg);
        EXPECT_EQ(msg._object_id, data._detected_object_common_data._object_id);
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::UNKNOWN, data._detected_object_common_data._object_type);
        EXPECT_EQ(90, data._detected_object_common_data._classification_confidence);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._x*10), data._detected_object_common_data._position_offset._offset_x);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._y*10), data._detected_object_common_data._position_offset._offset_y);
        EXPECT_EQ(static_cast<unsigned int>(msg._position._y*10), data._detected_object_common_data._position_offset._offset_y);
        EXPECT_EQ(115, data._detected_object_common_data._speed);
        EXPECT_EQ(260, data._detected_object_common_data._speed_z);
        EXPECT_EQ(static_cast<unsigned int>(msg._size._length*10), std::get<streets_utils::messages::sdsm::detected_obstacle_data>(data._detected_object_optional_data.value())._size._length);


    }

    TEST(detectedObjectToSdsmConverterTest, toSdsmTimestampTest) {
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

    TEST(detectedObjectToSdsmConverterTest, toPositionConfidenceSet) {
        // Covariance matrix x variance 0.4m variance =0.4m and z variance 0.4m (diagonal)
        std::vector<std::vector<double>> position_covariance = {{0.2, 2.3, 5.2},{0.1, 0.2, 5.2},{0.1, 2.3, 0.2}};
        auto position_confidence_set = to_position_confidence_set(position_covariance);
        // 0.4m * 2 for 95 % confidence interval is closer to 1 m than to 50 cm
        EXPECT_EQ( streets_utils::messages::sdsm::position_confidence::A_1M ,position_confidence_set._position_confidence );
        EXPECT_EQ( streets_utils::messages::sdsm::position_confidence::A_1M ,position_confidence_set._elevation_confidence );

    }

    TEST(detectedObjectToSdsmConvertTest, toPositionConfidence) {
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_500M, to_position_confidence(550));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_500M, to_position_confidence(375));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_500M, to_position_confidence(350));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_200M, to_position_confidence(349));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_200M, to_position_confidence(150));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_100M, to_position_confidence(149));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_100M, to_position_confidence(75));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_50M, to_position_confidence(74));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_50M, to_position_confidence(35));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_20M, to_position_confidence(34));

        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_500M, to_position_confidence(500));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_200M, to_position_confidence(200));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_100M, to_position_confidence(100));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_50M, to_position_confidence(50));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_20M, to_position_confidence(20));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_10M, to_position_confidence(10));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_5M, to_position_confidence(5));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_2M, to_position_confidence(2));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_1M, to_position_confidence(1));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_50CM, to_position_confidence(.50));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_20CM, to_position_confidence(.20));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_10CM, to_position_confidence(.10));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_5CM, to_position_confidence(.05));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_2CM, to_position_confidence(.02));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_1CM, to_position_confidence(.01));
        EXPECT_EQ(streets_utils::messages::sdsm::position_confidence::A_1CM, to_position_confidence(.001));

    }


    TEST(detectedObjectToSdsmConvertTest, toHeading) {
        streets_utils::messages::detected_objects_msg::vector_3d velocity;
        velocity._x = 0;
        velocity._y= 0;
        EXPECT_EQ(0, to_heading(velocity));
        velocity._x = 1;
        velocity._y= 0;
        EXPECT_EQ(0, to_heading(velocity));
        velocity._x = 1;
        velocity._y = 1;
        EXPECT_EQ(3600, to_heading(velocity));
        velocity._x = 0;
        velocity._y = 1;
        EXPECT_EQ(7200, to_heading(velocity));
        velocity._x = -1;
        velocity._y = 1;
        EXPECT_EQ(10800, to_heading(velocity));
        velocity._x = -1;
        velocity._y = 0;
        EXPECT_EQ(14400, to_heading(velocity));
        velocity._x = -1;
        velocity._y = -1;
        EXPECT_EQ(18000, to_heading(velocity));
        velocity._x = 0;
        velocity._y = -1;
        EXPECT_EQ(21600, to_heading(velocity));
        velocity._x = 1;
        velocity._y = -1;
        EXPECT_EQ(25200, to_heading(velocity));
        

    }

    TEST(detectedObjectToSdsmConvertTest, toSpeed_XYZ_Confidence) {
        std::vector<std::vector<double>> velocity_covariance = {{4, 2.3, 5.2},{0.1, 4, 5.2},{0.1, 2.3, 9}};
        auto speed_confidence =  to_xy_speed_confidence(velocity_covariance);
        auto speed_z_confidence = to_z_speed_confidence(velocity_covariance);
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_5ms,speed_z_confidence);
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_5ms,speed_confidence);

    }

    TEST(detectedObjectToSdsmConvertTest, toSpeedConfidence) {
        
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_100ms,to_speed_confidence(100));
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_10ms,to_speed_confidence(10));
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_5ms,to_speed_confidence(5));
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_1ms,to_speed_confidence(1));
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_0_1ms,to_speed_confidence(0.1));
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_0_05ms,to_speed_confidence(0.05));
        EXPECT_EQ(streets_utils::messages::sdsm::speed_confidence::PREC_0_01ms,to_speed_confidence(0.01));


    }

    TEST(detectedObjectToSdsmConvertTest, toYawRateConfidence) {
        std::vector<std::vector<double>> angular_velocity_covariance = {{4, 2.3, 5.2},{0.1, 4, 5.2},{0.1, 2.3, 9}};
        auto speed_confidence =  to_yaw_rate_confidence(angular_velocity_covariance);
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_05,speed_confidence);
        angular_velocity_covariance = {{0.01, 0.0, 0.0},{0.0, 0.01, 0.0},{0.0, 0.0, 0.01}};
        speed_confidence =  to_yaw_rate_confidence(angular_velocity_covariance);
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_0_1,speed_confidence);

    }

    TEST(detectedObjectToSdsmConvertTest, toAngularVelocityConfidence) {
        
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_100,to_angular_velocity_confidence(100));
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_10,to_angular_velocity_confidence(10));
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_05,to_angular_velocity_confidence(5));
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_01,to_angular_velocity_confidence(1));
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_0_1,to_angular_velocity_confidence(0.1));
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_0_05,to_angular_velocity_confidence(0.05));
        EXPECT_EQ(streets_utils::messages::sdsm::angular_velocity_confidence::DEGSEC_0_01,to_angular_velocity_confidence(0.01));

    }

    TEST(detectedObjectToSdsmConvertTest, toYawRate) {
        EXPECT_NEAR(2939, to_yaw_rate(0.513), 1);
        EXPECT_NEAR(-2939, to_yaw_rate(-0.513), 1);

    }


}