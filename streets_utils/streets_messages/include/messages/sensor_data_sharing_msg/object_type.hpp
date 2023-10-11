#pragma once

namespace streets_utils::messages{
    
    enum class object_type {
        UNKNOWN = 0,
        VEHICLE = 1,
        VRU= 2,
        ANIMAL= 3        
    };

    inline object_type object_type_from_int( const int i ) {
        switch (i)
        {
        case 0:
            return object_type::UNKNOWN;
            break;
        case 1:
            return object_type::VEHICLE;
            break;
        case 2:
            return object_type::VRU;
            break;
        case 3:
            return object_type::ANIMAL;
            break;
        default:
            return object_type::UNKNOWN;
        }
    };
}