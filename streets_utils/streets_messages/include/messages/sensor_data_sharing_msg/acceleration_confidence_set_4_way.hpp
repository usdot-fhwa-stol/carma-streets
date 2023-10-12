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

#include "acceleration_confidence.hpp"
#include "angular_velocity_confidence.hpp"


namespace streets_utils::messages{
    struct acceleration_confidence_set_4_way{
        /**
         * @brief Confidence in acceleration reported along longitudinal axis.
         */
        acceleration_confidence _longitudinal_confidence;
        /**
         * @brief Confidence in acceleration reported along lateral axis.
         */
        acceleration_confidence _lateral_confidence;
        /**
         * @brief Confidence in acceleration reported along vertical access.
         */
        acceleration_confidence _vertical_confidence;
        /**
         * @brief Confidence in turning angular velocity reported.
         */
        angular_velocity_confidence _yaw_rate_confidence;
    };
}