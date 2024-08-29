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
#pragma once 

#include <streets_utils/json_utils_lib/json_utils.hpp>
// Lanelet2 libraries
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <lanelet2_core/primitives/Polygon.h>
#include <lanelet2_core/utility/Units.h>
#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/primitives/BoundingBox.h>
#include <lanelet2_extension/projection/mgrs_projector.h>
#include <lanelet2_extension/projection/local_frame_projector.h>
#include <lanelet2_extension/io/autoware_osm_parser.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_io/io_handlers/Factory.h>
#include <lanelet2_core/geometry/Point.h>
#include <lanelet2_projection/UTM.h>
#include <map>
#include <stdexcept>
#include <shared_mutex>

#include <streets_utils/json_utils_lib/json_utils.hpp>

namespace sensor_data_sharing_service{
    /**
     * @brief Enumeration to describe location data type (CARTESIAN or WGS84). NOTE: only use CARTESIAN in simulation enviroments. 
     */
    enum class LocationDataType {
        WGS84 = 0,
        CARTESIAN = 1,
    };
    
    /**
     * @brief Describes sensor reference location to intepret detection data.
     */
    struct SensorReference {
        std::string sensorId = "";
        /**
         * @brief Enumeration to describe location data type (CARTESIAN or WGS84). NOTE: only use CARTESIAN in simulation enviroments. 
         */
        LocationDataType reference_type;
        /**
         * @brief WGS84 reference location for sensor. Only read when reference type is WGS84
         */
        lanelet::GPSPoint wgs84_location;
        /**
         * @brief Cartesian reference location for sensor. Interpreted from lanet2 osm map. Only read when reference type is CARTESIAN
         * NOTE: only use in simuatlion enviroment.
         */
        lanelet::BasicPoint3d cartesian_location;
    };


     /**
     * @brief Method to parse sensor configuration json file.
     * @param filepath absolute or relative file path.
     * @param sensor_id ID of sensor to get location from.
     * @return SensorReference of sensor described in json configuration file.
     * @throws streets_utils::json_utils::json_parse_exception if JSON file is malformed or sensor id is not found in file.
     */
    SensorReference parse_sensor_ref( const std::string &filepath, const std::string &sensor_id );

    
}