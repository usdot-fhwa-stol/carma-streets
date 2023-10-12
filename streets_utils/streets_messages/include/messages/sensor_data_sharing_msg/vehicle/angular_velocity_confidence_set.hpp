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

#include "../angular_velocity_confidence.hpp"
#include <optional>

namespace streets_utils::messages{
    struct angular_velocity_confidence_set{
        /**
         * @brief Confidence in reported pitch rate.
         */
        std::optional<angular_velocity_confidence> _pitch_rate_confidence;
        /**
         * @brief Confidence in reported roll rate.
         */
        std::optional<angular_velocity_confidence> _roll_rate_confidence;
    };
}