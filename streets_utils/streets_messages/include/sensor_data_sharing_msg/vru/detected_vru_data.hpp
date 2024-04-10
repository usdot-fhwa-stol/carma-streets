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

#include "sensor_data_sharing_msg/vru/human_propelled_type.hpp"
#include "sensor_data_sharing_msg/vru/animal_propelled_type.hpp"
#include "sensor_data_sharing_msg/vru/motorized_propelled_type.hpp"
#include "sensor_data_sharing_msg/vru/attachment.hpp"
#include "sensor_data_sharing_msg/vru/personal_device_user_type.hpp"

#include <variant>
#include <optional>

namespace streets_utils::messages::sdsm{
    struct detected_vru_data{
        /**
         * @brief Propulsion type.
         */
        std::optional<std::variant<human_propelled_type, motorized_propelled_type, animal_propelled_type>> _propulsion;
        /**
         * @brief Attachment type
         */
        std::optional<attachment> _attachment;
        /**
         * @brief Attachment radius in decimeters [0,200]
         */
        std::optional<unsigned int>  _attachment_radius;
        /**
         * @brief Personal device user type
         */
        std::optional<personal_device_user_type> _personal_device_user_type;
    };
}