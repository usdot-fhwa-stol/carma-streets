#pragma once
#include "stdlib.h"

namespace streets_utils::messages{
    struct position_offset{
        /**
         * @brief Cartesian offset in X axis from reference point in 0.1 m [-32767, 32767]
         */
       std::int16_t _offset_x;
        /**
         * @brief Cartesian offset in Y axis from reference point in 0.1 m [-32767, 32767]
         */
       std::int16_t _offset_y;
        /**
         * @brief Cartesian offset in Z axis from reference point in 0.1 m [-32767, 32767]
         */
        std::int16_t _offset_z;

    };
}