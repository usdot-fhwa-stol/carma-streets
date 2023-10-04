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

    position_confidence position_confidence_from_int( const int i );


}