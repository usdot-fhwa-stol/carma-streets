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

#include <streets_utils/streets_messages_lib/detected_object_msg/detected_object_msg.hpp>

namespace sensor_data_sharing_service { 
    /**
     * @brief Converts detected_objects_msg from ENU coordinate frame to NED.
     * @param msg 
     * @return Returns detected_objects_msg in NED coordinate frame.
     */
    streets_utils::messages::detected_objects_msg::detected_objects_msg detected_object_enu_to_ned(const streets_utils::messages::detected_objects_msg::detected_objects_msg &msg );
}