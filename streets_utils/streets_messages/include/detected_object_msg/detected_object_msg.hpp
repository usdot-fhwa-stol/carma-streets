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

#include <detection_type.hpp>
#include <cartesian_point.hpp>
#include <vector_3d.hpp>
#include <size.hpp>

namespace streets_utils::messages {

    namespace detected_object_msg {
        /**
         * @brief Sensor Detected Object information
         */
        struct detected_object_msg {

            std::string type;
            double confidence;
            std::string projString;
            std::string objectID;
            cartesian_point position;
            double positionCovariance[3][3];
            vector_3d velocity;
            double velocityCovariance[3][3];
            vector_3d angularVelocity;
            double angularVelocityCovariance[3][3];
            size size;

        };
    }

}