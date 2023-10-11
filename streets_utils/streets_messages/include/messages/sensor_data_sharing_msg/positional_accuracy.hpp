#pragma once

#include <stdlib.h>

namespace streets_utils::messages {
    struct positional_accuracy{
        /**
         * @brief semi-major axis accuracy at one standard dev 
         * range 0-12.7 meter, LSB = .05m [0, 255]
         * 254 = any value equal or greater than 12.70 meter
         * 255 = unavailable semi-major axis value
         */
        unsigned int  _semi_major_axis_accuracy;
        /**
         * @brief semi-minor axis accuracy at one standard dev 
         * range 0-12.7 meter, LSB = .05m [0, 255]
         * 254 = any value equal or greater than 12.70 meter
         * 255 = unavailable semi-minor axis value
         */
        unsigned int  _semi_minor_axis_accuracy;
        /**
         * @brief -- orientation of semi-major axis 
         * relative to true north (0~359.9945078786 degrees)
         * LSB units of 360/65535 deg  = 0.0054932479 [0,65535]
         * a value of 0 shall be 0 degrees
         * a value of 1 shall be 0.0054932479 degrees 
         * a value of 65534 shall be 359.9945078786 deg
         * a value of 65535 shall be used for orientation unavailable
         */
        unsigned int  _semi_major_axis_orientation;
    };
}