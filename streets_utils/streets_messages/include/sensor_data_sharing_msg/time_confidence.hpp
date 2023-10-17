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
    enum class time_confidence{
        UNAVAILABLE = 0,            // unvailable
        TIME_100_000 = 1,           // Better than 100 seconds
        TIME_050_000 = 2,           // Better than 50 seconds
        TIME_020_000 = 3,           // Better than 20 seconds
        TIME_010_000 = 4,           // Better than 10 seconds
        TIME_002_000 = 5,           // Better than 2 seconds
        TIME_001_000 = 6,           // Better than 1 seconds
        TIME_000_500 = 7,           // Better than 0.5 seconds
        TIME_000_200 = 8,           // Better than 0.2 seconds
        TIME_000_100 = 9,           // Better than 0.1 seconds
        TIME_000_050 = 10,          // Better than 0.05 seconds
        TIME_000_020 = 11,          // Better than 0.02 seconds
        TIME_000_010 = 12,          // Better than 0.01 seconds
        TIME_000_005 = 13,          // Better than 0.005 seconds
        TIME_000_002 = 14,          // Better than 0.002 seconds
        TIME_000_001 = 15,          // Better than 0.001 seconds
        TIME_000_000_5 = 16,        // Better than 0.0005 seconds
        TIME_000_000_2 = 17,        // Better than 0.0002 seconds
        TIME_000_000_1 = 18,        // Better than 0.0001 seconds
        TIME_000_000_05 = 19,       // Better than 0.00005 seconds
        TIME_000_000_02 = 20,       // Better than 0.00002 seconds
        TIME_000_000_01 = 21,       // Better than 0.00001 seconds
        TIME_000_000_005 = 22,      // Better than 0.000005 seconds
        TIME_000_000_002 = 23,      // Better than 0.000002 seconds
        TIME_000_000_001 = 24,      // Better than 0.000001 seconds
        TIME_000_000_000_5 = 25,    // Better than 0.0000005 seconds
        TIME_000_000_000_2 = 26,    // Better than 0.0000002 seconds
        TIME_000_000_000_1 = 27,    // Better than 0.0000001 seconds
        TIME_000_000_000_05 = 28,   // Better than 0.00000005 seconds
        TIME_000_000_000_02 = 29,   // Better than 0.00000002 seconds
        TIME_000_000_000_01 = 30,   // Better than 0.00000001 seconds
        TIME_000_000_000_005 = 31,  // Better than 0.000000005 seconds
        TIME_000_000_000_002 = 32,  // Better than 0.000000002 seconds
        TIME_000_000_000_001 = 33,  // Better than 0.000000001 seconds (Better than  one nano second) 
        TIME_000_000_000_000_5 = 34,// Better than 0.0000000005 seconds
        TIME_000_000_000_000_2 = 35,// Better than 0.0000000002 seconds
        TIME_000_000_000_000_1 = 36,// Better than 0.0000000001 seconds
        TIME_000_000_000_000_05 = 37, // Better than 0.00000000005 seconds
        TIME_000_000_000_000_02 = 38, // Better than 0.00000000002 seconds
        TIME_000_000_000_000_01 = 39  // Better than 0.00000000001 Seconds    
    };

    inline time_confidence time_confidence_from_int( const int i ) {
        switch (i)
        {
        case 0:
            return time_confidence::UNAVAILABLE;
        case 1:
            return time_confidence::TIME_100_000;
        case 2:           
            return time_confidence::TIME_050_000 ;
        case 3:
            return time_confidence::TIME_020_000 ;
        case 4:
            return time_confidence::TIME_010_000 ;
        case 5:
            return time_confidence::TIME_002_000 ;
        case 6:
            return time_confidence::TIME_001_000 ;
        case 7:
            return time_confidence::TIME_000_500 ;
        case 8:
            return time_confidence::TIME_000_200 ;
        case 9:
            return time_confidence::TIME_000_100 ;
        case 10:
            return time_confidence::TIME_000_050 ;
        case 11:
            return time_confidence::TIME_000_020 ;
        case 12:
            return time_confidence::TIME_000_010 ;
        case 13:
            return time_confidence::TIME_000_005 ;
        case 14:
            return time_confidence::TIME_000_002 ;
        case 15:
            return time_confidence::TIME_000_001 ;
        case 16:
            return time_confidence::TIME_000_000_5;
        case 17:
            return time_confidence::TIME_000_000_2;
        case 18:
            return time_confidence::TIME_000_000_1;
        case 19:
            return time_confidence::TIME_000_000_05;
        case 20:
            return time_confidence::TIME_000_000_02;
        case 21:
            return time_confidence::TIME_000_000_01;
        case 22:
            return time_confidence::TIME_000_000_005;
        case 23:
            return time_confidence::TIME_000_000_002;
        case 24:
            return time_confidence::TIME_000_000_001;
        case 25:
            return time_confidence::TIME_000_000_000_5;
        case 26:
            return time_confidence::TIME_000_000_000_2;
        case 27:
            return time_confidence::TIME_000_000_000_1;
        case 28:
            return time_confidence::TIME_000_000_000_05;
        case 29:
            return time_confidence::TIME_000_000_000_02;
        case 30:
            return time_confidence::TIME_000_000_000_01;
        case 31:
            return time_confidence::TIME_000_000_000_005;
        case 32:
            return time_confidence::TIME_000_000_000_002;
        case 33:
            return time_confidence::TIME_000_000_000_001; 
        case 34:
            return time_confidence::TIME_000_000_000_000_5;
        case 35:
            return time_confidence::TIME_000_000_000_000_2;
        case 36:
            return time_confidence::TIME_000_000_000_000_1;
        case 37:
            return time_confidence::TIME_000_000_000_000_05;
        case 38:
            return time_confidence::TIME_000_000_000_000_02;
        case 39:
            return time_confidence::TIME_000_000_000_000_01;
        default:
            return time_confidence::UNAVAILABLE; 
        }
    };

}