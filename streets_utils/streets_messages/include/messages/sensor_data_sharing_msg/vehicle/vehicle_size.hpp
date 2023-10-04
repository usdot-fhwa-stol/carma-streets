#pragma once

#include<stdlib.h>

namespace streets_utils::messages{
    struct vehicle_size {
        /**
         * @brief Vehicle width in centimeters [0, 1023]
         */
        u_int16_t _width;
        /**
         * @brief Vehicle length in centimeters [0, 4095]
         */
        u_int16_t _length; 
        
    };
}