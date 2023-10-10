#pragma once

#include <stdlib.h>

namespace streets_utils::messages {
    struct time_stamp {
        /**
         * @brief in milliseconds [0,65535].
         */
        uint second;
        /**
         * @brief in minutes [0,60].
         */
        uint minute;
        /**
         * @brief in hours [0,31].
         */
        uint hour;
        /**
         * @brief in days [0,31].
         */
        uint day;
        /**
         * @brief in months [0,12].
         */
        uint month;
        /**
         * @brief in year [0,4095]
         */
        uint year;
        /**
         * @brief Minutes from UTC time (Time Zone) [-840, 840]
         */
        int offset; // Time zone
    };
}