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
    struct acceleration_set_4_way{
        /**
         * @brief Longitudinal acceleration in 0.01 m/s^s [-2000, 2001]
         */
        int _longitudinal_accel;
        /**
         * @brief Lateral acceleration in 0.01 m/s^s [-2000, 2001]
         */
        int _lateral_accel;
        /**
         * @brief Vertical acceleration in 0.02 G [-127, 127]
         */
        int _vertical_accel;   
        /**
         * @brief Angular velocity in 0.01 degrees [-32767, 32767]
         */
        int _yaw_rate;
    };
}