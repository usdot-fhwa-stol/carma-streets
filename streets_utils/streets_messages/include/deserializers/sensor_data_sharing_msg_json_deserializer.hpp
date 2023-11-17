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
#pragma once
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/rapidjson.h>
#include <string>
#include <streets_utils/json_utils_lib/json_utils.hpp>
#include <streets_utils/json_utils_lib/json_utils_exception.hpp>
#include "sensor_data_sharing_msg/sensor_data_sharing_msg.hpp"

namespace streets_utils::messages::sdsm {
    sensor_data_sharing_msg from_json( const std::string &val);

    time_stamp parse_time_stamp(const rapidjson::Value &val);

    position_3d parse_position_3d(const rapidjson::Value &val);

    positional_accuracy parse_positional_accuracy(const rapidjson::Value &val);

    std::optional<position_confidence> parse_elevation_confidence(const rapidjson::Value &val);

    std::vector<detected_object_data> parse_detected_object_list(const rapidjson::Value &val);

    detected_object_data_common parse_detected_object_data_common(const rapidjson::Value &val);

    acceleration_set_4_way parse_acceleration_4_way(const rapidjson::Value &val);

    std::optional<std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data>>  parse_detected_object_data_optional(const rapidjson::Value &val);

    position_offset parse_position_offset(const rapidjson::Value &val);

    position_confidence_set parse_position_confidence_set(const rapidjson::Value &val);

    detected_obstacle_data parse_detected_obstacle_data(const rapidjson::Value &val);

    detected_vehicle_data parse_detected_vehicle_data(const rapidjson::Value &val);

    detected_vru_data parse_detected_vru_data(const rapidjson::Value &val);

    obstacle_size parse_obstacle_size(const rapidjson::Value &val);

    obstacle_size_confidence parse_obstacle_size_confidence(const rapidjson::Value &val);

    std::optional<std::variant<human_propelled_type, motorized_propelled_type, animal_propelled_type>> parse_propelled_information( const rapidjson::Value &val);

    attitude parse_vehicle_attitude(const rapidjson::Value &val);

    attitude_confidence parse_vehicle_attitude_confidence(const rapidjson::Value &val);

    angular_velocity_set parse_vehicle_angular_velocity_set(const rapidjson::Value &val);

    angular_velocity_confidence_set parse_vehicle_angular_velocity_confidence_set(const rapidjson::Value &val);

    vehicle_size parse_vehicle_size(const rapidjson::Value &val);

    vehicle_size_confidence parse_vehicle_size_confidence(const rapidjson::Value &val);
}