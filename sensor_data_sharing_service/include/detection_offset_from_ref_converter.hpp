// Copyright 2026 Leidos
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
#include <proj.h>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <optional>

namespace sensor_data_sharing_service { 
    /**
     * @brief Parses a whitespace-separated PROJ string (e.g. "+proj=utm +zone=10 +datum=WGS84")
     * into a map of parameter name to value. Each token has any leading '+' stripped; tokens
     * containing '=' are split into a key/value pair, while flag-only tokens (no '=') are
     * stored with an empty string value.
     * @param proj_str PROJ definition string to parse
     * @return Unordered map of PROJ parameter names to their values (empty string if the
     * parameter is a flag with no value)
     */
    std::unordered_map<std::string, std::string> parse_proj_string(const std::string &proj_str);
    /**
     * @brief Transforms a detected object's cartesian position from its local/detection PROJ
     * frame into the given reference PROJ frame, using the PROJ library to build and apply the
     * CRS-to-CRS transformation. Returns a copy of msg with the updated position and its
     * _proj_string set to reference_proj_string. If the PROJ context or transformation cannot
     * be created (e.g. invalid or incompatible proj strings), the returned object's position is
     * set to (0.0, 0.0) instead.
     * @param msg Detected object with a local position and its originating PROJ string
     * @param reference_proj_string PROJ string of the shared reference frame to transform into
     * @return Copy of msg repositioned into the reference frame (or zeroed out on transform failure)
     */
    streets_utils::messages::detected_objects_msg::detected_objects_msg detected_object_local_to_ref(const streets_utils::messages::detected_objects_msg::detected_objects_msg &msg, const std::string &reference_proj_string);
}