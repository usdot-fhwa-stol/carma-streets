#pragma once

#include <stdlib.h>

namespace streets_utils::messages{
    struct acceleration_set_4_way{
        /**
         * @brief Longitudinal acceleration in 0.01 m/s^s [-2000, 2001]
         */
        std::int16_t _longitudinal_accel;
        /**
         * @brief Lateral acceleration in 0.01 m/s^s [-2000, 2001]
         */
        std::int16_t _lateral_accel;
        /**
         * @brief Vertical acceleration in 0.02 G [-127, 127]
         */
        int8_t _vertical_accel;   
        /**
         * @brief Angular velocity in 0.01 degrees [-32767, 32767]
         */
        std::int16_t _yaw_rate;
    };
}