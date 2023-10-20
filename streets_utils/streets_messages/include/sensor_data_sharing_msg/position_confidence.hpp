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
    enum class position_confidence{
        UNAVAILABLE = 0,
        A_500M = 1, 
        A_200M = 2,
        A_100M = 3,
        A_50M = 4,
        A_20M = 5,
        A_10M = 6,
        A_5M = 7,
        A_2M = 8,
        A_1M = 9,
        A_50CM = 10,
        A_20CM = 11,
        A_10CM = 12,
        A_5CM = 13,
        A_2CM = 14,
        A_1CM = 15
        
    };

    inline position_confidence position_confidence_from_int( const int i ) {
        switch (i)
        {
        case 0:
            return position_confidence::UNAVAILABLE;
        case 1:
            return position_confidence::A_500M;
        case 2:
            return position_confidence::A_200M;
        case 3:
            return position_confidence::A_100M;
        case 4:
            return position_confidence::A_50M;
        case 5:
            return position_confidence::A_20M;
        case 6:
            return position_confidence::A_10M;
        case 7:
            return position_confidence::A_5M;
        case 8:
            return position_confidence::A_2M;
        case 9:
            return position_confidence::A_1M;
        case 10:
            return position_confidence::A_50CM;
        case 11:
            return position_confidence::A_20CM;
        case 12:
            return position_confidence::A_10CM;
        case 13:
            return position_confidence::A_5CM;
        case 14:
            return position_confidence::A_2CM;
        case 15:
            return position_confidence::A_1CM;
        default:
            throw std::invalid_argument("Incompatible position confidence value. Valid values : [0,15]");
        }
    };


}