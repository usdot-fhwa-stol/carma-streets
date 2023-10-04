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

    human_propelled_type human_propelled_type_from_int( const int i );

}