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
#include "deserializers/detected_obj_msg_deserializer.hpp"

#include <iostream>

namespace streets_utils::messages::detected_objects_msg {

    detected_objects_msg from_json( const std::string &json)
    {
        // Deserializes the incoming json message
        rapidjson::Document document = streets_utils::json_utils::parse_json(json);
        detected_objects_msg msg;

        msg._type = streets_utils::json_utils::parse_string_member("type",document, true).value();
        msg._confidence = streets_utils::json_utils::parse_double_member("confidence", document, true).value();
        msg._sensor_id = streets_utils::json_utils::parse_string_member("sensorId", document, true).value();
        msg._proj_string = streets_utils::json_utils::parse_string_member("projString", document, true).value();
        msg._object_id = streets_utils::json_utils::parse_string_member("objectId", document, true).value();
        std::cout<<"Reaching line 31"<<std::endl;
        auto position_obj = streets_utils::json_utils::parse_object_member("position", document, true).value();
        msg._position = parse_cartesian_3d(position_obj);
        std::cout<<"Reaching line 34"<<std::endl;
        auto position_cov_obj = streets_utils::json_utils::parse_array_member("positionCovariance", document, true).value();
        msg._position_covariance = parse_covariance(position_cov_obj);
        std::cout<<"Reaching line 37"<<std::endl;
        auto velocity_obj = streets_utils::json_utils::parse_object_member("velocity", document, true).value();
        msg._velocity = parse_vector3d(velocity_obj);
        std::cout<<"Reaching line 40"<<std::endl;

        auto velocity_cov_obj = streets_utils::json_utils::parse_array_member("velocityCovariance", document, true).value();
        msg._velocity_covariance = parse_covariance(velocity_cov_obj);

        auto angular_velocity_obj = streets_utils::json_utils::parse_object_member("angularVelocity", document, true).value();
        msg._angular_velocity = parse_vector3d(angular_velocity_obj);

        auto angular_velocity_cov_obj = streets_utils::json_utils::parse_array_member("angularVelocityCovariance", document, true).value();
        msg._angular_velocity_covariance = parse_covariance(angular_velocity_cov_obj);
        std::cout<<"Reaching line 50"<<std::endl;
        auto size_obj = streets_utils::json_utils::parse_object_member("size", document, true).value();
        msg._size = parse_size(size_obj);
        std::cout<<"Reaching line 53"<<std::endl;
    }

    cartesian_point parse_cartesian_3d(const rapidjson::Value &val)
    {
        std::cout<<"Entering parse_cartesian_3d"<<std::endl;
        cartesian_point point;
        point._x = streets_utils::json_utils::parse_double_member("x", val, true).value();
        point._y = streets_utils::json_utils::parse_double_member("y", val, true).value();
        point._z = streets_utils::json_utils::parse_double_member("z", val, true).value();
        std::cout<<"Exiting parse_cartesian_3d"<<std::endl;
        return point;
    }

    vector_3d parse_vector3d(const rapidjson::Value &val)
    {
        std::cout<<"Entering parse_vector_3d"<<std::endl;
        vector_3d vector;
        vector._x = streets_utils::json_utils::parse_double_member("x", val, true).value();
        vector._y = streets_utils::json_utils::parse_double_member("y", val, true).value();
        vector._z = streets_utils::json_utils::parse_double_member("z", val, true).value();
        std::cout<<"Exiting parse_vector_3d"<<std::endl;
        return vector;

    }

    std::vector<std::vector<double>> parse_covariance(const rapidjson::Value::ConstArray &val)
    {
        // Initialize 3x3 covariance matrix
        std::vector<std::vector<double>> covariance;

        for (size_t i = 0; i < val.Size(); i++)
        {
            const rapidjson::Value& row = val[i];
            std::vector<double> val_row;

            for (size_t j = 0; j < row.Size(); j++)
            {
                val_row.push_back(val[i][j].GetDouble());
            }
            covariance.push_back(val_row);
        }
        return covariance;
    }

    size parse_size(const rapidjson::Value &val)
    {
        size size_obj;
        size_obj._length = streets_utils::json_utils::parse_double_member("length", val, true).value();
        size_obj._height = streets_utils::json_utils::parse_double_member("height", val, true).value();
        size_obj._width = streets_utils::json_utils::parse_double_member("width", val, true).value();

        return size_obj;
    }


}