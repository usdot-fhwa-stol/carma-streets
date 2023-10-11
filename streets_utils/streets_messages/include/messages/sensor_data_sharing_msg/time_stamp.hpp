#pragma once

#include <stdlib.h>

namespace streets_utils::messages {
    struct time_stamp {
        /**
         * @brief in milliseconds [0,65535].
         */
        unsigned int  second;
        /**
         * @brief in minutes [0,60].
         */
        unsigned int  minute;
        /**
         * @brief in hours [0,31].
         */
        unsigned int  hour;
        /**
         * @brief in days [0,31].
         */
        unsigned int  day;
        /**
         * @brief in months [0,12].
         */
        unsigned int  month;
        /**
         * @brief in year [0,4095]
         */
        unsigned int  year;
        /**
         * @brief Minutes from UTC time (Time Zone) [-840, 840]
         */
        int offset; // Time zone
    };
}