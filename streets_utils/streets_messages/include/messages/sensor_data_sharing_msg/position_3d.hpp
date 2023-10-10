#pragma once

#include <stdlib.h>
#include <optional>

namespace streets_utils::messages{
    struct position_3d{
        /**
         * @brief LSB = 1/10 micro degree Providing a range of 
         * plus-minus 180 degrees[-1799999999, 1800000001]
         */
        int _longitude;
        /**
         * @brief LSB = 1/10 micro degree Providing a range of 
         * plus-minus 90 degrees[-900000000, 900000001]
         */
        int _latitude;
        /**
         * @brief Signed units of 0.1m (10cm), in 2 octets the value 
         * 32767 (0x7FFF) shall indicate an invalid value [-32768,32767]
         */
        std::optional<int> _elavation;

    };
}