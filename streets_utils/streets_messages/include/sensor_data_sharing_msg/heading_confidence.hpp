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

namespace streets_utils::messages{
    enum class heading_confidence{
        UNAVAILABLE = 0,
        PREC_10_deg = 1,        // 10 degrees
        PREC_05_deg = 2,        // 5 degrees
        PREC_01_deg = 3,        // 1 degrees
        PREC_0_1_deg = 4,       // 0.1 degrees
        PREC_0_05_deg = 5,      // 0.05 degrees
        PREC_0_01_deg = 6,      // 0.01 degrees
        PREC_0_0125_deg = 7     // 0.0125 degrees
    };

    inline heading_confidence heading_confidence_from_int( const unsigned int i ) {
        switch (i)
        {
        case 0:
            return heading_confidence::UNAVAILABLE;
        case 1:
            return heading_confidence::PREC_10_deg;
        case 2:
            return heading_confidence::PREC_05_deg;
        case 3:
            return heading_confidence::PREC_01_deg;
        case 4:
            return heading_confidence::PREC_0_1_deg;
        case 5:
            return heading_confidence::PREC_0_05_deg;
        case 6:
            return heading_confidence::PREC_0_01_deg;
        case 7:
            return heading_confidence::PREC_0_0125_deg;
        default:
            throw std::invalid_argument("Incompatible heading confidence value. Valid values : [0,7]");
        }
    };


}