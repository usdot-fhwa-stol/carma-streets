#pragma once

#include <stdlib.h>

namespace streets_utils::messages{
    struct attitude{
        /**
         * @brief Pitch in 0.0125 degrees [-7200, 72000].
         */
        std::int16_t _pitch;
        /**
         * @brief Roll in 0.0125 degrees [-14400, 14400] 
         */
        std::int16_t _roll;
        /**
         * @brief Yaw in 0.0125 degrees [-14400, 14400] 
         */
        std::int16_t _yaw;
    };
}