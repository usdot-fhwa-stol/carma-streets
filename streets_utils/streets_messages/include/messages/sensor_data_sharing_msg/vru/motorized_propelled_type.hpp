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
    
    inline motorized_propelled_type motorized_propelled_type_from_int( const unsigned int i ){
        switch (i)
        {
        case 0:
            return motorized_propelled_type::UNAVAILABLE;
        case 1:
            return motorized_propelled_type::OTHER_TYPES;
        case 2:           
            return motorized_propelled_type::WHEEL_CHAIR;
        case 3:
            return motorized_propelled_type::BICYCLE;
        case 4:
            return motorized_propelled_type::SCOOTER;
        case 5:
            return motorized_propelled_type::SELF_BALANCING_DEVICE;
        default:           
            return motorized_propelled_type::UNAVAILABLE;
        }
    };

}