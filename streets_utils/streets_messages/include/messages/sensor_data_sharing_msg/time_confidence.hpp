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

    time_confidence time_confidence_from_int( const int i );

}