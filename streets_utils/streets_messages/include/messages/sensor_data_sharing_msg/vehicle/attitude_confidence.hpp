#pragma once

#include "../heading_confidence.hpp"

namespace streets_utils::messages{
    struct attitude_confidence {
        /**
         * @brief Confidence in reported pitch.
         */
        heading_confidence _pitch_confidence;
        /**
         * @brief Confidence in reported roll.
         */
        heading_confidence _roll_confidence;
        /**
         * @brief Confidence in reported yaw.
         */
        heading_confidence _yaw_confidence;
    };
}