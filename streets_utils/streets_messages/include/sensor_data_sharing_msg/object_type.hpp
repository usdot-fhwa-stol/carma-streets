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
        case 1:
            return object_type::VEHICLE;
        case 2:
            return object_type::VRU;
        case 3:
            return object_type::ANIMAL;
        default:
            throw std::invalid_argument("Incompatible object type value. Valid values : [0,3]");
        }
    };
}