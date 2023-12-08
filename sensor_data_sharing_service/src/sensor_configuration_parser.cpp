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

    lanelet::BasicPoint3d parse_sensor_location( const std::string &filepath , const std::string &sensor_id ){
        // Parse JSON configuration file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open Streets configuration file " + filepath + " !"); 
        }
        // Add file contents to stream and parse stream into Document
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document doc;
        doc.ParseStream(isw);
        if (doc.HasParseError()){
            SPDLOG_ERROR("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
            throw std::runtime_error("Encounter document parse error while attempting to parse sensor configuration file " + filepath + "!");
        }
        file.close();
        if (!doc.IsArray()) {
            throw std::runtime_error("Invadid format for sensor configuration file "  + filepath + ".");
        }
        else {
            bool found = false;
            lanelet::BasicPoint3d sensor_location;
            for (auto itr = doc.Begin(); itr != doc.End(); ++itr) {
                // Access the data in the object
                auto _sensor_id = streets_utils::json_utils::parse_string_member("sensorId",  itr->GetObject(), true ).value();
                if ( _sensor_id == sensor_id ) {
                    found = true;
                    auto location = streets_utils::json_utils::parse_object_member("location",  itr->GetObject(), true ).value();
                    sensor_location = lanelet::BasicPoint3d{
                        streets_utils::json_utils::parse_double_member("x",  location, true ).value(),
                        streets_utils::json_utils::parse_double_member("y",  location, true ).value(),
                        streets_utils::json_utils::parse_double_member("z",  location, true ).value()
                        };
                }
            }
            if (!found) {
                throw std::runtime_error("Did not find sensor with id " + sensor_id + " in sensor configuration file " + filepath + "!");
            }
            return sensor_location;
        }
       
    }
}