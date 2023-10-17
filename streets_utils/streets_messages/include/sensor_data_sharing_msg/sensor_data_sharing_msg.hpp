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

#include "sensor_data_sharing_msg/equipment_type.hpp"
#include "sensor_data_sharing_msg/time_stamp.hpp"
#include "sensor_data_sharing_msg/detected_object_data.hpp"
#include "sensor_data_sharing_msg/position_3d.hpp"
#include "sensor_data_sharing_msg/positional_accuracy.hpp"

#include <vector>
#include <string>
#include <optional>

namespace streets_utils::messages {


    /**
     * @brief 
     */
    struct sensor_data_sharing_msg {
        /**
         * @brief -- a count value which is incremented with each use [0,255]
         *  the next value after 255 shall be one value 
         *  0 (0x00) shall indicate that MsgCount is not available.
         */
        unsigned int  _msg_count;
        equipment_type _equipment_type;
        position_3d _ref_positon;
        std::optional<position_confidence> _ref_position_elavation_confidence;
        positional_accuracy _ref_position_confidence;
        time_stamp _time_stamp;
        std::string _source_id;
        std::vector<detected_object_data> _objects;
                
    };

}