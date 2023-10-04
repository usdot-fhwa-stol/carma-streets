#pragma once

#include "human_propelled_type.hpp"
#include "animal_proppelled_type.hpp"
#include "motorized_propelled_type.hpp"
#include "attachment.hpp"

#include <variant>
#include <stdlib.h>

namespace streets_utils::messages{
    struct detected_vru_data{
        /**
         * @brief Propulsion type.
         */
        std::variant<human_propelled_type, motorized_propelled_type, animal_propelled_type> _propulsion;
        /**
         * @brief Attachment type
         */
        attachment _attachment;
        /**
         * @brief Attachment radius in decimeters [0,200]
         */
        u_int8_t _attachment_radious;
    };
}