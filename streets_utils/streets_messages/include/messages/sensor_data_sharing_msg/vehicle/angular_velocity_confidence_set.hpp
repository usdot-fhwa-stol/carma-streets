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