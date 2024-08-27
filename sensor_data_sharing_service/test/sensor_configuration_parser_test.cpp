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
    // TEST(sensor_configuration_parser_test, parse_sensor_location_test) {
    //     auto pose = parse_sensor_location("/home/carma-streets/sensor_data_sharing_service/test/test_files/sensors.json", "sensor_1"); 
    //     EXPECT_NEAR(pose.x(), 1.0 , 0.01 );
    //     EXPECT_NEAR(pose.y(), 2.0 , 0.01 );
    //     EXPECT_NEAR(pose.z(), -3.2 , 0.01 );

    // }

    TEST(sensor_configuration_parser_test, parse_sensor_cartesian_test) {
        auto ref = parse_sensor_ref("/home/carma-streets/sensor_data_sharing_service/test/test_files/sensors_cartesian.json", "sensor_1"); 
        EXPECT_EQ(ref.map_projection, "" );
        EXPECT_EQ(ref.reference_type, ReferenceType::CARTESIAN);
        EXPECT_NEAR(ref.location.x(), 1.0 , 0.01 );
        EXPECT_NEAR(ref.location.y(), 2.0 , 0.01 );
        EXPECT_NEAR(ref.location.z(), -3.2 , 0.01 );

    }

    TEST(sensor_configuration_parser_test, parse_sensor_georef_test) {
        auto ref = parse_sensor_ref("/home/carma-streets/sensor_data_sharing_service/test/test_files/sensors_georef.json", "sensor_1");

        EXPECT_EQ(ref.map_projection, "+proj=tmerc +lat_0=0 +lon_0=0 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +geoidgrids=egm96_15.gtx +vunits=m +no_defs" );
        EXPECT_EQ(ref.reference_type, ReferenceType::GEO_REF);
        EXPECT_NEAR(ref.location.x(), 0.0 , 0.01 );
        EXPECT_NEAR(ref.location.y(), 0.0 , 0.01 );
        EXPECT_NEAR(ref.location.z(), 0.0 , 0.01 );


    }
}