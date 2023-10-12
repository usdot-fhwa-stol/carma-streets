#pragma once

namespace streets_utils::messages{

    enum class animal_propelled_type{
        UNAVAILABLE = 0,
        OTHER_TYPES = 1,
        ANIMAL_MOUNTED = 2,
        ANIMAL_DRAWN_CARRIAGE = 3,
    };

    inline animal_propelled_type animal_propelled_type_from_int( const unsigned int i ) {
        switch (i)
        {
        case 0:
            return animal_propelled_type::UNAVAILABLE;
        case 1:
            return animal_propelled_type::OTHER_TYPES;
        case 2:           
            return animal_propelled_type::ANIMAL_MOUNTED;
        case 3:
            return animal_propelled_type::ANIMAL_DRAWN_CARRIAGE;
        default:           
            return animal_propelled_type::UNAVAILABLE;
        }
    };

}