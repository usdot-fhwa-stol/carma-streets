#pragma once

namespace streets_utils::messages {

    enum class human_propelled_type{
        UNAVAILABLE = 0,
        OTHER_TYPES = 1,
        ON_FOOT = 2,
        SKATEBOARD = 3,
        PUSH_OR_KICK_SCOOTER = 4,
        WHEELCHAIR = 5
    };

    inline human_propelled_type human_propelled_type_from_int( const int i ){
        switch (i)
        {
        case 0:
            return human_propelled_type::UNAVAILABLE;
        case 1:
            return human_propelled_type::OTHER_TYPES;
        case 2:           
            return human_propelled_type::ON_FOOT;
        case 3:
            return human_propelled_type::SKATEBOARD;
        case 4:
            return human_propelled_type::PUSH_OR_KICK_SCOOTER;
        case 5:
            return human_propelled_type::WHEELCHAIR;
        default:           
            return human_propelled_type::UNAVAILABLE;
        }
    };

}