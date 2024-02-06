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
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/rapidjson.h>
#include <string>
#include <streets_utils/json_utils_lib/json_utils.hpp>
#include <streets_utils/json_utils_lib/json_utils_exception.hpp>
#include <detected_object_msg/detected_object_msg.hpp>
#include <stdexcept>



namespace streets_utils::messages::detected_objects_msg {


    detected_objects_msg from_json( const std::string &val);

    cartesian_point parse_cartesian_3d(const rapidjson::Value &val);

    std::vector<std::vector<double>>  parse_covariance(const rapidjson::Value::ConstArray &val);

    vector_3d parse_vector3d(const rapidjson::Value &val);

    size parse_size(const rapidjson::Value &val);


}