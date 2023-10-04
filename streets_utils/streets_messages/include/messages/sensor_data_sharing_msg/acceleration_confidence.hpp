#pragma once

namespace streets_utils::messages{
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

    inline acceleration_confidence acceleration_confidence_from_int( const int i ) {
        switch (i)
        {
        case 0:
            return acceleration_confidence::UNAVAILABLE;
            break;
        case 1:
            return acceleration_confidence::ACCL_100;
            break;
        case 2:
            return acceleration_confidence::ACCL_10;
            break;
        case 3:
            return acceleration_confidence::ACCL_5;
            break;
        case 4:
            return acceleration_confidence::ACCL_1;
            break;
        case 5:
            return acceleration_confidence::ACCL_0_1;
            break;
        case 6:
            return acceleration_confidence::ACCL_0_05;
            break;
        case 7:
            return acceleration_confidence::ACCL_0_01;
            break;
        default:
            return acceleration_confidence::UNAVAILABLE;
        }
    };

}