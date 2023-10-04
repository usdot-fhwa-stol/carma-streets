#pragma once

namespace streets_utils::messages{
    
    enum class object_type {
        UNKNOWN = 0,
        VEHICLE = 1,
        VRU= 2,
        ANIMAL= 3        
    };

    object_type object_type_from_int( const int i );
}