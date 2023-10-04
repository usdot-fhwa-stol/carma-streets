#pragma once

#include <stdlib.h>

namespace streets_utils::messages {
    struct positional_accuracy{
        std::uint8_t _semi_major_axis_accuracy;
        std::uint8_t _semi_minor_axis_accuracy;
        std::uint16_t _semi_major_axis_orientation;
    };
}