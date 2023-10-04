#pragma once 
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>
#include <rapidjson/rapidjson.h>
#include <string>
#include <json_utils.hpp>

#include "messages/sensor_data_sharing_msg/sensor_data_sharing_msg.hpp"

namespace streets_utils::messages {
    sensor_data_sharing_msg from_json( const std::string &val);
}