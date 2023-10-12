// Copyright 2023 Leidos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

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