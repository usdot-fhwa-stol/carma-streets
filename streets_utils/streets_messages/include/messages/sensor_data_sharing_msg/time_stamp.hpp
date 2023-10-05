#pragma once

#include <stdlib.h>

namespace streets_utils::messages {
    struct time_stamp {
        /**
         * @brief in milliseconds [0,65535].
         */
        std::uint16_t second;
        /**
         * @brief in minutes [0,60].
         */
        std::uint8_t minute;
        /**
         * @brief in hours [0,31].
         */
        std::uint8_t hour;
        /**
         * @brief in days [0,31].
         */
        std::uint8_t day;
        /**
         * @brief in months [0,12].
         */
        std::uint8_t month;
        /**
         * @brief in year [0,4095]
         */
        std::uint16_t year;
        /**
         * @brief Minutes from UTC time (Time Zone) [-840, 840]
         */
        std::int16_t offset; // Time zone
    };
}