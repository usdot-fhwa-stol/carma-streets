#pragma once

#include "stdlib.h"

namespace streets_utils::messages {
    struct obstable_size {
        /**
         * @brief Object width in 10 cm units [0, 1023].
         */
        uint _width;
        /**
         * @brief Object length in 10 cm units [0, 1023]
         */
        uint _length;
        /**
         * @brief Object height in 10 cm units [0, 1023]
         */
        uint _height;
    };
}