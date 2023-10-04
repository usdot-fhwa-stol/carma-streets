#pragma once

namespace streets_utils::messages {

    enum class personal_device_user_type{
        UNAVAILABLE= 0,
        PEDESTRIAN = 1,
        PEDALCYCLIST = 2,
        PUBLIC_SAFETY_WORKER = 3,
        ANIMAL = 4
    };

    personal_device_user_type personal_device_user_type_from_int( const int i );

}