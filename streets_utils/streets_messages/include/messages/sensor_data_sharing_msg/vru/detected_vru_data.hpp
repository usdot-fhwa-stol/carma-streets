#pragma once

#include "human_propelled_type.hpp"
#include "animal_proppelled_type.hpp"
#include "motorized_propelled_type.hpp"
#include "attachment.hpp"
#include "personal_device_user_type.hpp"

#include <variant>
#include <optional>

namespace streets_utils::messages{
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
        std::optional<unsigned int>  _attachment_radious;
        /**
         * @brief Personal device user type
         */
        std::optional<personal_device_user_type> _personal_device_user_type;
    };
}