#pragma once

#include <stdlib.h>

namespace streets_utils::messages {
    struct positional_accuracy{
        u_int8_t _semi_major_axis_accuracy;
        u_int8_t _semi_minor_axis_accuracy;
        u_int16_t _semi_major_axis_orientation;
    };
}