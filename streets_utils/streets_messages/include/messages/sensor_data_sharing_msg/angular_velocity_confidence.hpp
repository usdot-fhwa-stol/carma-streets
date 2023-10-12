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
    enum class angular_velocity_confidence{
        UNAVAILABLE = 0,
        DEGSEC_100 = 1,         // 100 degrees
        DEGSEC_10 = 2,          // 10 degrees
        DEGSEC_05 = 3,          // 5 degrees
        DEGSEC_01 = 4,          // 1 degrees
        DEGSEC_0_1 = 5,         // 0.1 degrees
        DEGSEC_0_05 = 6,        // 0.05 degrees
        DEGSEC_0_01 = 7,        // 0.01 degrees
    };

    inline angular_velocity_confidence angular_velocity_confidence_from_int( const unsigned int i ){
        switch (i)
        {
        case 0:
            return angular_velocity_confidence::UNAVAILABLE;
        case 1:
            return angular_velocity_confidence::DEGSEC_100;
        case 2:
            return angular_velocity_confidence::DEGSEC_10;
        case 3:
            return angular_velocity_confidence::DEGSEC_05;
        case 4:
            return angular_velocity_confidence::DEGSEC_01;
        case 5:
            return angular_velocity_confidence::DEGSEC_0_1;
        case 6:
            return angular_velocity_confidence::DEGSEC_0_05;
        case 7:
            return angular_velocity_confidence::DEGSEC_0_01;
        default:
            return angular_velocity_confidence::UNAVAILABLE;
        }
    };


}