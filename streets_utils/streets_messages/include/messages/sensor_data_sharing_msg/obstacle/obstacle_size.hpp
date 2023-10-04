#pragma once

#include "stdlib.h"

namespace streets_utils::messages {
    struct obstable_size {
        /**
         * @brief Object width in 10 cm units [0, 1023].
         */
        u_int16_t _width;
        /**
         * @brief Object length in 10 cm units [0, 1023]
         */
        u_int16_t _length;
        /**
         * @brief Object height in 10 cm units [0, 1023]
         */
        u_int16_t _height;
    };
}