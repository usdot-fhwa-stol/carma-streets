#pragma once

#include "optional"

namespace streets_utils::messages {
    struct obstable_size {
        /**
         * @brief Object width in 10 cm units [0, 1023].
         */
        unsigned int  _width;
        /**
         * @brief Object length in 10 cm units [0, 1023]
         */
        unsigned int  _length;
        /**
         * @brief **Optional** Object height in 10 cm units [0, 1023]
         */
        std::optional<unsigned int>  _height;
    };
}