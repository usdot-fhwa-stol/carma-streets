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

#include "messages/sensor_data_sharing_msg/obstacle/obstacle_size.hpp"
#include "messages/sensor_data_sharing_msg/obstacle/obstacle_size_confidence.hpp"
namespace streets_utils::messages{
    struct detected_obstacle_data{
        /**
         * @brief Size of obstacle.
         */
        obstacle_size _size;
        /**
         * @brief Confidence of reported size.
         */
        obstacle_size_confidence _size_confidence;
    };
}