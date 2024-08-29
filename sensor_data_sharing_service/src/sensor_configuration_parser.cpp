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
#include "sensor_configuration_parser.hpp"

namespace sensor_data_sharing_service {

    SensorReference parse_sensor_ref( const std::string &filepath , const std::string &sensor_id ){
        // Parse JSON configuration file
        SensorReference sensor_ref;
        auto doc = streets_utils::json_utils::parse_json_file(filepath);
        if (!doc.IsArray()) {
            throw streets_utils::json_utils::json_parse_exception("Invadid format for sensor configuration file "  + filepath 
                + ". Sensor configuration file should contain an array of json sensor configurations!");
        }
        bool found = false;
        for (auto itr = doc.Begin(); itr != doc.End(); ++itr) {
            // Access the data in the object
            auto sensor_config_id = streets_utils::json_utils::parse_string_member("sensorId",  itr->GetObject(), true ).value();
            if ( sensor_config_id == sensor_id ) {
                found = true; 
                auto ref = streets_utils::json_utils::parse_object_member("ref",  itr->GetObject(), true ).value();
                auto ref_type = streets_utils::json_utils::parse_string_member("type", ref, true).value();
                auto location = streets_utils::json_utils::parse_object_member("location",  ref, true ).value();
                if ( ref_type == "CARTESIAN") {
                    sensor_ref.reference_type = LocationDataType::CARTESIAN;
                    sensor_ref.cartesian_location = lanelet::BasicPoint3d{
                        streets_utils::json_utils::parse_double_member("x",  location, true ).value(),
                        streets_utils::json_utils::parse_double_member("y",  location, true ).value(),
                        streets_utils::json_utils::parse_double_member("z",  location, true ).value()
                        };
                }
                else { 
                    sensor_ref.reference_type = LocationDataType::WGS84;
                    sensor_ref.wgs84_location = lanelet::GPSPoint{
                        streets_utils::json_utils::parse_double_member("lat", location, true).value(),
                        streets_utils::json_utils::parse_double_member("lon", location, true).value(),
                        streets_utils::json_utils::parse_double_member("elevation", location, true).value(),

                    };
                }
            }
        }
        if (!found) {
            throw streets_utils::json_utils::json_parse_exception("Did not find sensor with id " + sensor_id + " in sensor configuration file " + filepath + "!");
        }
        return sensor_ref;
    }

}