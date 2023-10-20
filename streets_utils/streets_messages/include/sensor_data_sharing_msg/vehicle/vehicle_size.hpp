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
    struct vehicle_size {
        /**
         * @brief Vehicle width in centimeters [0, 1023]
         */
        unsigned int  _width = 0;
        /**
         * @brief Vehicle length in centimeters [0, 4095]
         */
        unsigned int  _length = 0; 
        
    };
}