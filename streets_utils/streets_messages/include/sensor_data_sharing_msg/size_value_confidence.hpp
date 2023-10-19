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
    enum class size_value_confidence{
        UNAVAILABLE = 0,    // Not available
        SIZE_100 = 1,       // 100 meters
        SIZE_50 = 2,        // 50 meters
        SIZE_20 = 3,        // 20 meters
        SIZE_10 = 4,        // 10 meters
        SIZE_5 = 5,         // 5 meters
        SIZE_2 = 6,         // 2 meters
        SIZE_1 = 7,         // 1 meter
        SIZE_0_5 = 8,       // 50 centimeters
        SIZE_0_2 = 9,       // 20 centimeters
        SIZE_0_1 = 10,      // 10 centimeters
        SIZE_0_05 = 11,     // 5 centimeters
        SIZE_0_02 = 12,     // 2 centimeters
        SIZE_0_01 = 13      // 1 centimeters  
    };

    inline size_value_confidence size_value_confidence_from_int( const unsigned int i ) {
        switch (i)
        {
        case 0:
            return size_value_confidence::UNAVAILABLE;
        case 1:
            return size_value_confidence::SIZE_100;
        case 2:           
            return size_value_confidence::SIZE_50;
        case 3:
            return size_value_confidence::SIZE_20;
        case 4:
            return size_value_confidence::SIZE_10;
        case 5:
            return size_value_confidence::SIZE_5;
        case 6:
            return size_value_confidence::SIZE_2;
        case 7:           
            return size_value_confidence::SIZE_1;
        case 8:
            return size_value_confidence::SIZE_0_5;
        case 9:
            return size_value_confidence::SIZE_0_2;
        case 10:
            return size_value_confidence::SIZE_0_1;
        case 11:
            return size_value_confidence::SIZE_0_05;
        case 12:           
            return size_value_confidence::SIZE_0_02;
        case 13:
            return size_value_confidence::SIZE_0_01;
        default:
            throw std::invalid_argument("Incompatible size confidence value. Valid values : [0,13]");
        }
    } 
}