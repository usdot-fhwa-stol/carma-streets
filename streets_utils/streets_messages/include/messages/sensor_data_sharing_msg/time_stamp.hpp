#pragma once

#include <stdlib.h>

namespace streets_utils::messages {
    struct time_stamp {
        std::uint8_t second;
        std::uint8_t minute;
        std::uint8_t hour;
        std::uint8_t day;
        std::uint8_t month;
        std::uint16_t year;
        std::uint8_t offset; // Time zone
    };
}