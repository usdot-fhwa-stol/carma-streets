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

    inline heading_confidence heading_confidence_from_int( const int i ) {
        switch (i)
        {
        case 0:
            return heading_confidence::UNAVAILABLE;
            break;
        case 1:
            return heading_confidence::PREC_10_deg;
            break;
        case 2:
            return heading_confidence::PREC_05_deg;
            break;
        case 3:
            return heading_confidence::PREC_01_deg;
            break;
        case 4:
            return heading_confidence::PREC_0_1_deg;
            break;
        case 5:
            return heading_confidence::PREC_0_05_deg;
            break;
        case 6:
            return heading_confidence::PREC_0_01_deg;
            break;
        case 7:
            return heading_confidence::PREC_0_0125_deg;
            break;
        default:
            return heading_confidence::UNAVAILABLE;
        }
    };


}