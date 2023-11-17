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

namespace streets_utils::messages::sdsm{

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
            throw std::invalid_argument("Incompatible animal propelled type value. Valid values : [0,3]");
        }
    };

}