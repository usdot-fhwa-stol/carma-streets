#pragma once

#include <string>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "sensor_data_sharing_msg/sensor_data_sharing_msg.hpp"
#include <streets_utils/json_utils_lib/json_utils_exception.hpp>


namespace streets_utils::messages::sdsm{
    std::string to_json(const sensor_data_sharing_msg &val);

    rapidjson::Value create_timestamp(const time_stamp &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_position_3d(const position_3d &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_positional_accuracy(const positional_accuracy &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_detected_object_list(const std::vector<detected_object_data> &val, rapidjson::Document::AllocatorType &allocator );

    rapidjson::Value create_detected_object_data(const detected_object_data &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_detected_object_data_common(const detected_object_data_common &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_detected_object_data_optional(const std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data> &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_detected_obstacle_data(const detected_obstacle_data &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_obstacle_size(const obstacle_size &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_obstacle_size_confidence(const obstacle_size_confidence &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_detected_vru_data(const detected_vru_data &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_propelled_information(const std::variant<human_propelled_type, motorized_propelled_type, animal_propelled_type> &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_detected_vehicle_data(const detected_vehicle_data &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_vehicle_attitude(const attitude &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_vehicle_attitude_confidence(const attitude_confidence &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_angular_velocity(const angular_velocity_set &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_angular_velocity_confidence(const angular_velocity_confidence_set &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_vehicle_size(const vehicle_size &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_vehicle_size_confidence(const vehicle_size_confidence &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_accelaration_set_4_way(const acceleration_set_4_way &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_position_3d(const position_offset &val, rapidjson::Document::AllocatorType &allocator);

    rapidjson::Value create_position_confidence_set(const position_confidence_set &val, rapidjson::Document::AllocatorType &allocator);
}