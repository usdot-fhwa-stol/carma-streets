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

namespace streets_utils::messages::sdsm{
    enum class acceleration_confidence {
        UNAVAILABLE = 0,    // Not available
        ACCL_100 = 1,     // 100 m/s^2
        ACCL_10 = 2,      // 10 m/s^2
        ACCL_5 = 3,       // 5 m/s^2
        ACCL_1 = 4,       // 1 m/s^2
        ACCL_0_1 = 5,     // 0.1 m/s^2
        ACCL_0_05 = 6,    // 0.05 m/s^2
        ACCL_0_01 = 7     // 0.01 m/s^2
    };

    inline acceleration_confidence acceleration_confidence_from_int( const unsigned int i ) {
        switch (i)
        {
        case 0:
            return acceleration_confidence::UNAVAILABLE;
        case 1:
            return acceleration_confidence::ACCL_100;
        case 2:
            return acceleration_confidence::ACCL_10;
        case 3:
            return acceleration_confidence::ACCL_5;
        case 4:
            return acceleration_confidence::ACCL_1;
        case 5:
            return acceleration_confidence::ACCL_0_1;
        case 6:
            return acceleration_confidence::ACCL_0_05;
        case 7:
            return acceleration_confidence::ACCL_0_01;
        default:
            throw std::invalid_argument("Incompatible acceleration confidence value. Valid values : [0,7]");
        }
    };

}