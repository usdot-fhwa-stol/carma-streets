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
#include "sensor_data_sharing_msg/size_value_confidence.hpp"
#include <optional>
namespace streets_utils::messages::sdsm{
    struct vehicle_size_confidence {
        /**
         * @brief Confidence in reported width
         */
        size_value_confidence _width_confidence = size_value_confidence::UNAVAILABLE;
        /**
         * @brief Confidence in reported length
         */
        size_value_confidence _length_confidence = size_value_confidence::UNAVAILABLE;
        /**
         * @brief Confidence in reported height
         */
        std::optional<size_value_confidence> _height_confidence;
    };
}