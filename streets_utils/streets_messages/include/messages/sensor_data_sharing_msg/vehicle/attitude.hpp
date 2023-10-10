#pragma once

#include <stdlib.h>

namespace streets_utils::messages{
    struct attitude{
        /**
         * @brief Pitch in 0.0125 degrees [-7200, 72000].
         */
        int _pitch;
        /**
         * @brief Roll in 0.0125 degrees [-14400, 14400] 
         */
        int _roll;
        /**
         * @brief Yaw in 0.0125 degrees [-14400, 14400] 
         */
        int _yaw;
    };
}