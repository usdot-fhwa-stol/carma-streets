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

#include<optional>

namespace streets_utils::messages{
    struct position_offset{
      /**
       * @brief Cartesian offset in X axis from reference point in 0.1 m [-32767, 32767]
       */
      int _offset_x;
      /**
       * @brief Cartesian offset in Y axis from reference point in 0.1 m [-32767, 32767]
       */
      int _offset_y;
      /**
       * @brief Cartesian offset in Z axis from reference point in 0.1 m [-32767, 32767]
       */
      std::optional<int> _offset_z;

    };
}