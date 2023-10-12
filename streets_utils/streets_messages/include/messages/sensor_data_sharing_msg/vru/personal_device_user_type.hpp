// Copyright 2023 Leidos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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