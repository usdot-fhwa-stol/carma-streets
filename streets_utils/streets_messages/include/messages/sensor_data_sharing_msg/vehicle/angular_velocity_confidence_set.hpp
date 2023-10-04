#pragma once

#include "../angular_velocity_confidence.hpp"

namespace streets_utils::messages{
    class angular_velocity_confidence_set{
        private: 
            /**
             * @brief Confidence in reported pitch rate.
             */
            angular_velocity_confidence _pitch_rate_confidence;
            /**
             * @brief Confidence in reported roll rate.
             */
            angular_velocity_confidence _roll_rate_confidence;
    };
}