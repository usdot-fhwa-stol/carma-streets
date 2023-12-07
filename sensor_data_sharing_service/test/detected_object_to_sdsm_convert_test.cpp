// Copyright 2019-2023 Leidos
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
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spdlog/spdlog.h>
#include <detected_object_to_sdsm_converter.hpp>
#include <streets_utils/streets_messages_lib/sensor_data_sharing_msg/object_type.hpp>

namespace sensor_data_sharing_service {
    TEST(detected_object_to_sdsm_convert_test, test_to_object_type){
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::VEHICLE, to_object_type("CAR"));
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::VRU, to_object_type("CYCLIST"));
        EXPECT_EQ(streets_utils::messages::sdsm::object_type::UNKNOWN, to_object_type("TREE"));
    }
}
