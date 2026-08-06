// Copyright 2026 Leidos
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
#include <detection_offset_from_ref_converter.hpp>

using ::testing::DoubleNear;

namespace sensor_data_sharing_service {
    TEST(DetectionOffsetFromRefConverterTest, TestMatchingPositions) {
        streets_utils::messages::detected_objects_msg::detected_objects_msg detection;
        
        std::string ref_string = "+proj=tmerc +lat_0=38.95197911150576 +lon_0=-77.14835128349988 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +geoidgrids=egm96_15.gtx +vunits=m +no_defs";

        auto pos_x =  2.0;
        auto pos_y = 3.0;
        auto pos_z =  5.0;
        auto vel_x = -3.2;
        auto vel_y = 0.7;
        auto vel_z = -5.0;

        detection._position._x = pos_x;
        detection._position._y = pos_y;
        detection._position._z = pos_z;
        detection._velocity._x = vel_x;
        detection._velocity._y = vel_y;
        detection._velocity._z = vel_z;
        detection._proj_string = "+proj=tmerc +lat_0=38.95197911150576 +lon_0=-77.14835128349988 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +geoidgrids=egm96_15.gtx +vunits=m +no_defs";

        auto updated_detection = detected_object_local_to_ref(detection, ref_string);
        EXPECT_DOUBLE_EQ(updated_detection._position._x, pos_x);
        EXPECT_DOUBLE_EQ(updated_detection._position._y, pos_y);
        EXPECT_DOUBLE_EQ(updated_detection._position._z, pos_z);

        EXPECT_DOUBLE_EQ(updated_detection._velocity._x, vel_x);
        EXPECT_DOUBLE_EQ(updated_detection._velocity._y, vel_y);
        EXPECT_DOUBLE_EQ(updated_detection._velocity._z, vel_z);
    }

    TEST(DetectionOffsetFromRefConverterTest, TestSmallDistance) {
        // Values pulled from known GNSS vehicle data
        streets_utils::messages::detected_objects_msg::detected_objects_msg detection;
        
        std::string ref_string = "+proj=tmerc +lat_0=38.95508624257387 +lon_0=-77.14738744642074 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +geoidgrids=egm96_15.gtx +vunits=m +no_defs";

        auto pos_x =  0.0;
        auto pos_y = 0.0;
        auto pos_z =  0.0;

        detection._position._x = pos_x;
        detection._position._y = pos_y;
        detection._position._z = pos_z;
        detection._proj_string = "+proj=tmerc +lat_0=38.95509339558609 +lon_0=-77.14738889727099 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +geoidgrids=egm96_15.gtx +vunits=m +no_defs";

        auto updated_detection = detected_object_local_to_ref(detection, ref_string);
        // Cartesian delta actuals:
        // x = -0.12576974393503
        // y = -0.79408763431725
        EXPECT_THAT(updated_detection._position._x, DoubleNear(-0.1258, 0.001));
        EXPECT_THAT(updated_detection._position._y, DoubleNear(0.7941, 0.001));
    
    }

    TEST(DetectionOffsetFromRefConverterTest, TestSmallDistanceWithOffset) {
        // Values pulled from known GNSS vehicle data
        streets_utils::messages::detected_objects_msg::detected_objects_msg detection;
        
        std::string ref_string = "+proj=tmerc +lat_0=38.95508624257387 +lon_0=-77.14738744642074 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +geoidgrids=egm96_15.gtx +vunits=m +no_defs";

        auto pos_x =  5.0;
        auto pos_y = 10.0;
        auto pos_z =  0.0;

        detection._position._x = pos_x;
        detection._position._y = pos_y;
        detection._position._z = pos_z;
        detection._proj_string = "+proj=tmerc +lat_0=38.95509339558609 +lon_0=-77.14738889727099 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +geoidgrids=egm96_15.gtx +vunits=m +no_defs";

        auto updated_detection = detected_object_local_to_ref(detection, ref_string);
        // Cartesian delta actuals:
        // x = -0.12576974393503
        // y = 0.79408763431725
        EXPECT_THAT(updated_detection._position._x, DoubleNear(4.8742, 0.001));
        EXPECT_THAT(updated_detection._position._y, DoubleNear(10.7941, 0.001));
    
    }
}