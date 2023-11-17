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

#include <vector>
#include <string>
#include <optional>


namespace streets_utils::messages::sdsm::detected_object_msg {

    /**
    * @brief Struct for size dimensions.
    */
    struct size{

        double _length; /**length in meters */
        double _height; /**height in meters */
        double _width;  /**width in meters */
    };

}