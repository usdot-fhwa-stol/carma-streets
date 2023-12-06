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
#include <streets_utils/streets_messages_lib/sensor_data_sharing_msg/sensor_data_sharing_msg.hpp>
#include <boost/date_time/posix_time/posix_time.hpp> //include all types plus i/o
#include <spdlog/spdlog.h>

namespace sensor_data_sharing_service {

    streets_utils::messages::sdsm::time_stamp to_sdsm_timestamp(const uint64_t _epoch_time_ms);

    streets_utils::messages::sdsm::detected_object_data to_detected_object_data(const streets_utils::messages::detected_objects_msg::detected_objects_msg &msg);
}