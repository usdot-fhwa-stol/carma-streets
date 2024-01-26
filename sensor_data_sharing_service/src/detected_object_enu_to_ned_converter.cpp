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
#include "detected_object_enu_to_ned_converter.hpp"

namespace sensor_data_sharing_service {

    streets_utils::messages::detected_objects_msg::detected_objects_msg detected_object_enu_to_ned(const streets_utils::messages::detected_objects_msg::detected_objects_msg &msg ) {
        streets_utils::messages::detected_objects_msg::detected_objects_msg ned_detection(msg);
        auto x = msg._position._y;
        auto y = msg._position._x;
        auto z = - msg._position._z;
        ned_detection._position._x = x;
        ned_detection._position._y = y;
        ned_detection._position._z = z;
        auto vel_x = msg._velocity._y;
        auto vel_y = msg._velocity._x;
        auto vel_z = -msg._velocity._z;
        ned_detection._velocity._x = vel_x;
        ned_detection._velocity._y = vel_y;
        ned_detection._velocity._z = vel_z; 
        return ned_detection;   
    }
}

