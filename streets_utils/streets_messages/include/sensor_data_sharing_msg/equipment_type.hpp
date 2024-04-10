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

    enum class equipment_type {
        UNKNOWN = 0,
        RSU = 1,
        OBU= 2,
        VRU= 3
    };
    /**
     * @brief Function to convert integers to equiment type. Default to UNKNOWN.
     * @param i integer value of enum.
     * @return corresponding equipement type.
     */
    inline equipment_type equipment_type_from_int( const unsigned int  i ) {
        switch (i)
        {
        case 0:
            return equipment_type::UNKNOWN;
        case 1:
            return equipment_type::RSU;
        case 2:
            return equipment_type::OBU;
        case 3:
            return equipment_type::VRU;
        default:
            throw std::invalid_argument("Incompatible equipment type value. Valid values : [0,3]");
        }
    };
}