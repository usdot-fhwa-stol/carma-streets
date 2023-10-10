#pragma once 
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/rapidjson.h>
#include <string>
#include <streets_utils/json_utils_lib/json_utils.hpp>
#include "messages/sensor_data_sharing_msg/sensor_data_sharing_msg.hpp"

namespace streets_utils::messages {
    sensor_data_sharing_msg from_json( const std::string &val);

    time_stamp parse_time_stamp(const rapidjson::Value &val);

    position_3d parse_position_3d(const rapidjson::Value &val);

    positional_accuracy parse_positional_accuracy(const rapidjson::Value &val);

    std::optional<position_confidence> parse_elevation_confidence(const rapidjson::Value &val);

    
}