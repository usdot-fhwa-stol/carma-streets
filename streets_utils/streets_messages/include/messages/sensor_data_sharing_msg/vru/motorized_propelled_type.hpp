#pragma once

namespace streets_utils::messages {
    enum class motorized_propelled_type{
        UNAVAILABLE = 0,
        OTHER_TYPES = 1,
        WHEEL_CHAIR = 2,
        BICYCLE = 3,
        SCOOTER = 4,
        SELF_BALANCING_DEVICE = 5
    };
    
    motorized_propelled_type motorized_propelled_type_from_int( const int i );

}