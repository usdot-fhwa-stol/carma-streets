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

#include <vector>
#include <string>
#include <optional>

#include <detected_object_msg/cartesian_point.hpp>
#include <detected_object_msg/vector_3d.hpp>
#include <detected_object_msg/size.hpp>

namespace streets_utils::messages::detected_objects_msg {

    /**
        * @brief Sensor Detected Object information
        */
    struct detected_objects_msg {

        std::string _type;
        double _confidence;
        std::string _sensor_id;
        std::string _proj_string;
        std::string _object_id;
        cartesian_point _position;
        std::vector<std::vector<double>> _position_covariance;
        vector_3d _velocity;
        std::vector<std::vector<double>> _velocity_covariance;
        vector_3d _angular_velocity;
        std::vector<std::vector<double>> _angular_velocity_covariance;
        size _size;

    };


}