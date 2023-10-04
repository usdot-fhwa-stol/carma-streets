#pragma once

namespace streets_utils::messages{

    enum class animal_propelled_type{
        UNAVAILABLE = 0,
        OTHER_TYPES = 1,
        ANIMAL_MOUNTED = 2,
        ANIMAL_DRAWN_CARRIAGE = 3,
    };

    animal_propelled_type animal_propelled_type_from_int( const int i );

}