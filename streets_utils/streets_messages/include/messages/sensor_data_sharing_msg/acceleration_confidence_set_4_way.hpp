#pragma once

#include "acceleration_confidence.hpp"
#include "angular_velocity_confidence.hpp"


namespace streets_utils::messages{
    struct acceleration_confidence_set_4_way{
        /**
         * @brief Confidence in acceleration reported along longitudinal axis.
         */
        acceleration_confidence _longitudinal_confidence;
        /**
         * @brief Confidence in acceleration reported along lateral axis.
         */
        acceleration_confidence _lateral_confidence;
        /**
         * @brief Confidence in acceleration reported along vertical access.
         */
        acceleration_confidence _vertical_confidence;
        /**
         * @brief Confidence in turning angular velocity reported.
         */
        angular_velocity_confidence _yaw_rate_confidence;
    };
}