#pragma once

namespace streets_utils::messages{
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

    speed_confidence speed_confidence_from_int( const int i );

}