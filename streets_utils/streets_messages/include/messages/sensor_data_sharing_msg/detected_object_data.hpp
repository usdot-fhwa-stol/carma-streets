#pragma once

#include "detected_object_data_common.hpp"
#include "obstacle/detected_obstacle_data.hpp"
#include "vehicle/detected_vehicle_data.hpp"
#include "vru/detected_vru_data.hpp"
#include <variant>

namespace streets_utils::messages{
    struct detected_object_data {
        /**
         * @brief Common data for detected object.
         */
        detected_object_data_common _detected_object_common_data;
        /**
         * @brief Detected object optional data associated with object type classification.
         */
        std::optional<std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data>> _detected_object_optional_data;
    };
}