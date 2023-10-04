#pragma once

#include <stdlib.h>
#include <optional>

namespace streets_utils::messages{
    struct position_3d{
        int64_t _longitude;
        int64_t _latitude;
        std::optional<int32_t> _elavation;

    };
}