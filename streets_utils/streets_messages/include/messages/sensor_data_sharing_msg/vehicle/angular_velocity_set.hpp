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
#include <stdlib.h>
namespace streets_utils::messages{
    struct angular_velocity_set{
        /**
         * @brief Angular velocity for pitch axis in 0.01 degrees per second [-32767, 32767]
         */
        int _pitch_rate;
        /**
         * @brief Angular velocity for roll axis in 0.01 degrees per second [-32767, 32767]
         */
        int _roll_rate;
    };
}