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

#include "sensor_data_sharing_msg/heading_confidence.hpp"

namespace streets_utils::messages{
    struct attitude_confidence {
        /**
         * @brief Confidence in reported pitch.
         */
        heading_confidence _pitch_confidence = heading_confidence::UNAVAILABLE;
        /**
         * @brief Confidence in reported roll.
         */
        heading_confidence _roll_confidence = heading_confidence::UNAVAILABLE;
        /**
         * @brief Confidence in reported yaw.
         */
        heading_confidence _yaw_confidence = heading_confidence::UNAVAILABLE;
    };
}