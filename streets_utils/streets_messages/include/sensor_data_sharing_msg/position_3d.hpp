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

#include <stdlib.h>
#include <optional>

namespace streets_utils::messages{
    struct position_3d{
        /**
         * @brief LSB = 1/10 micro degree Providing a range of 
         * plus-minus 180 degrees[-1799999999, 1800000001]
         */
        int _longitude = 0;
        /**
         * @brief LSB = 1/10 micro degree Providing a range of 
         * plus-minus 90 degrees[-900000000, 900000001]
         */
        int _latitude = 0;
        /**
         * @brief Signed units of 0.1m (10cm), in 2 octets the value 
         * 32767 (0x7FFF) shall indicate an invalid value [-32768,32767]
         */
        std::optional<int> _elevation;

    };
}