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
    enum class motorized_propelled_type{
        UNAVAILABLE = 0,
        OTHER_TYPES = 1,
        WHEEL_CHAIR = 2,
        BICYCLE = 3,
        SCOOTER = 4,
        SELF_BALANCING_DEVICE = 5
    };
    
    inline motorized_propelled_type motorized_propelled_type_from_int( const unsigned int i ){
        switch (i)
        {
        case 0:
            return motorized_propelled_type::UNAVAILABLE;
        case 1:
            return motorized_propelled_type::OTHER_TYPES;
        case 2:           
            return motorized_propelled_type::WHEEL_CHAIR;
        case 3:
            return motorized_propelled_type::BICYCLE;
        case 4:
            return motorized_propelled_type::SCOOTER;
        case 5:
            return motorized_propelled_type::SELF_BALANCING_DEVICE;
        default:           
            return motorized_propelled_type::UNAVAILABLE;
        }
    };

}