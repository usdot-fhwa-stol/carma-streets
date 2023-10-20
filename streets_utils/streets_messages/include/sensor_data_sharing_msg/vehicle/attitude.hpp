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
    struct attitude{
        /**
         * @brief Pitch in 0.0125 degrees [-7200, 72000].
         */
        int _pitch = 0;
        /**
         * @brief Roll in 0.0125 degrees [-14400, 14400] 
         */
        int _roll = 0;
        /**
         * @brief Yaw in 0.0125 degrees [-14400, 14400] 
         */
        int _yaw = 0;
    };
}