#pragma once

#include <stdlib.h>

namespace streets_utils::messages {
    struct time_stamp {
        u_int8_t second;
        u_int8_t minute;
        u_int8_t hour;
        u_int8_t day;
        u_int8_t month;
        u_int16_t year;
        u_int8_t offset; // Time zone
    };
}