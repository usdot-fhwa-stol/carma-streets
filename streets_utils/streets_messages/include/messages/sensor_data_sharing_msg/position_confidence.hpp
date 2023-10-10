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
            break;
        case 1:
            return position_confidence::A_500M;
            break;
        case 2:
            return position_confidence::A_200M;
            break;
        case 3:
            return position_confidence::A_100M;
            break;
        case 4:
            return position_confidence::A_50M;
            break;
        case 5:
            return position_confidence::A_20M;
            break;
        case 6:
            return position_confidence::A_10M;
            break;
        case 7:
            return position_confidence::A_5M;
            break;
        case 8:
            return position_confidence::A_2M;
            break;
        case 9:
            return position_confidence::A_1M;
            break;
        case 10:
            return position_confidence::A_50CM;
            break;
        case 11:
            return position_confidence::A_20CM;
            break;
        case 12:
            return position_confidence::A_10CM;
            break;
        case 13:
            return position_confidence::A_5CM;
            break;
        case 14:
            return position_confidence::A_2CM;
            break;
        case 15:
            return position_confidence::A_1CM;
            break;
        default:
            return position_confidence::UNAVAILABLE;
        }
    };


}