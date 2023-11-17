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
    enum class speed_confidence {
        UNAVAILABLE = 0,    // Not available
        PREC_100ms = 1,     // 100 m/s
        PREC_10ms = 2,      // 10 m/s
        PREC_5ms = 3,       // 5 m/s
        PREC_1ms = 4,       // 1 m/s
        PREC_0_1ms = 5,     // 0.1 m/s
        PREC_0_05ms = 6,    // 0.05 m/s
        PREC_0_01ms = 7     // 0.01 m/s
    };

    inline speed_confidence speed_confidence_from_int( const unsigned int i ) {
        switch (i)
        {
        case 0:
            return speed_confidence::UNAVAILABLE;
        case 1:
            return speed_confidence::PREC_100ms;
        case 2:
            return speed_confidence::PREC_10ms;
        case 3:
            return speed_confidence::PREC_5ms;
        case 4:
            return speed_confidence::PREC_1ms;
        case 5:
            return speed_confidence::PREC_0_1ms;
        case 6:
            return speed_confidence::PREC_0_05ms;
        case 7:
            return speed_confidence::PREC_0_01ms;
        default:
            throw std::invalid_argument("Incompatible speed confidence value. Valid values : [0,7]");
        }
    };

}