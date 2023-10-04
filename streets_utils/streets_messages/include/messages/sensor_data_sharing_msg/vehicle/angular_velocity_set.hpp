#pragma once
#include <stdlib.h>
namespace streets_utils::messages{
    struct angular_velocity_set{
        /**
         * @brief Angular velocity for pitch axis in 0.01 degrees per second [-32767, 32767]
         */
        std::int16_t _pitch_rate;
        /**
         * @brief Angular velocity for roll axis in 0.01 degrees per second [-32767, 32767]
         */
        std::int16_t _roll_rate;
    };
}