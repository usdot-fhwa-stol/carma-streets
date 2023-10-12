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