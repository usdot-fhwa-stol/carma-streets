#pragma once

namespace streets_utils::messages {

    enum class personal_device_user_type{
        UNAVAILABLE= 0,
        PEDESTRIAN = 1,
        PEDALCYCLIST = 2,
        PUBLIC_SAFETY_WORKER = 3,
        ANIMAL = 4
    };

    inline personal_device_user_type personal_device_user_type_from_int( const unsigned int i ) {
        switch (i)
        {
        case 0:
            return personal_device_user_type::UNAVAILABLE;
        case 1:
            return personal_device_user_type::PEDESTRIAN;
        case 2:           
            return personal_device_user_type::PEDALCYCLIST;
        case 3:
            return personal_device_user_type::PUBLIC_SAFETY_WORKER;
        case 4:
            return personal_device_user_type::ANIMAL;
        default:           
            return personal_device_user_type::UNAVAILABLE;
        }
    };

}