#pragma once

namespace streets_utils::messages{
    enum class size_value_confidence{
        UNAVAILALBE = 0,    // Not available
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
}