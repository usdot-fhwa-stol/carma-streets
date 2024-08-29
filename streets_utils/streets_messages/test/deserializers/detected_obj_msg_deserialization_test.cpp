// Copyright 2023 Leidos
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
#include <deserializers/detected_obj_msg_deserializer.hpp>
#include <string>

#include <iostream>

TEST(detected_obj_msg_deserializer_test, deserialize)
{
    std::string json_prediction = R"(
        {
            "type":"CAR",
            "confidence":0.7,
            "sensorId":"sensor1",
            "projString":"projectionString2",
            "objectId":27,
            "position":{
                "x":-1.1,
                "y":-2.0,
                "z":-3.2
            },
            "positionCovariance":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],
            "velocity":{
                "x":1.0,
                "y":1.0,
                "z":1.0
            },
            "velocityCovariance":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],
            "angularVelocity":{
                "x":0.1,
                "y":0.2,
                "z":0.3
            },
            "angularVelocityCovariance":[[1.0,0.0,0.0],[1.0,0.0,0.0],[1.0,0.0,0.0]],
            "size":{
                "length":2.0,
                "height":1.0,
                "width":0.5
            },
            "timestamp":1702335309
        }
        )";

    auto msg = streets_utils::messages::detected_objects_msg::from_json(json_prediction);
    EXPECT_EQ(msg._type, "CAR");
    EXPECT_NEAR(0.7,msg._confidence, 0.01);
    EXPECT_EQ(msg._sensor_id, "sensor1");
    EXPECT_EQ(msg._object_id, 27);
    EXPECT_NEAR(msg._position._x, -1.1, 0.001);
    EXPECT_NEAR(msg._position._y, -2.0, 0.001);
    EXPECT_NEAR(msg._position._z, -3.2, 0.001);
    // Position Covariance
    std::vector<std::vector<double>> pose_cov = {{1,0,0},{1,0,0},{1,0,0}};
    for (size_t i = 0 ; i < msg._position_covariance.size();i++)
    {
        for (size_t j=0; j< msg._position_covariance[i].size();j++)
        {
            EXPECT_NEAR(msg._position_covariance[i][j], pose_cov[i][j], 0.001);
        }

    }

    EXPECT_NEAR(msg._velocity._x, 1.0, 0.001);
    EXPECT_NEAR(msg._velocity._y, 1.0, 0.001);
    EXPECT_NEAR(msg._velocity._z,1.0, 0.001);
    // Velocity Covariance
    std::vector<std::vector<double>> vel_cov = {{1,0,0},{1,0,0},{1,0,0}};
        for (size_t i = 0 ; i < msg._velocity_covariance.size();i++)
    {
        for (size_t j=0; j< msg._velocity_covariance[i].size();j++)
        {
            EXPECT_NEAR(msg._velocity_covariance[i][j], vel_cov[i][j], 0.001);
        }

    }
    EXPECT_NEAR(msg._angular_velocity.value()._x,0.1, 0.001);
    EXPECT_NEAR(msg._angular_velocity.value()._y,0.2, 0.001);
    EXPECT_NEAR(msg._angular_velocity.value()._z,0.3, 0.001);
    // AngularVelocity Covariance
    std::vector<std::vector<double>> ang_vel_cov = {{1,0,0},{1,0,0},{1,0,0}};
        for (size_t i = 0 ; i < msg._angular_velocity_covariance.value().size();i++)
    {
        for (size_t j=0; j< msg._angular_velocity_covariance.value()[i].size();j++)
        {
            EXPECT_NEAR(msg._angular_velocity_covariance.value()[i][j], ang_vel_cov[i][j], 0.001);
        }

    }
    EXPECT_NEAR(msg._size._length,2.0, 0.001);
    EXPECT_NEAR(msg._size._height,1.0, 0.001);
    EXPECT_NEAR(msg._size._width,0.5, 0.001);
    EXPECT_EQ(msg._timestamp, 1702335309);

}