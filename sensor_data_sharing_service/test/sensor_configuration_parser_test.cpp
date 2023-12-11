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
#include <sensor_configuration_parser.hpp>


namespace sensor_data_sharing_service {
    TEST(sensor_configuration_parser_test, parse_sensor_location_test) {
        auto pose = parse_sensor_location("/home/carma-streets/sensor_data_sharing_service/test/test_files/sensors.json", "sensor_1"); 
        EXPECT_NEAR(pose.x(), 1.0 , 0.01 );
        EXPECT_NEAR(pose.y(), 2.0 , 0.01 );
        EXPECT_NEAR(pose.z(), -3.2 , 0.01 );

    }
}