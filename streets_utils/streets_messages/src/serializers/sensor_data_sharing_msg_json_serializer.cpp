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
#include "serializers/sensor_data_sharing_msg_json_serializer.hpp"


namespace streets_utils::messages{
    std::string to_json(const sensor_data_sharing_msg &msg) {
        rapidjson::Document doc;
        rapidjson::Value sdsm_json(rapidjson::kObjectType);
        sdsm_json.AddMember("msg_cnt", msg._msg_count, doc.GetAllocator());
        sdsm_json.AddMember("source_id", msg._source_id, doc.GetAllocator());
        sdsm_json.AddMember("equipment_type", static_cast<int>(msg._equipment_type), doc.GetAllocator());
        // Construct SDSM Time Stamp JSON Object
        auto time_stamp_json = create_timestamp(msg._time_stamp, doc.GetAllocator());
        sdsm_json.AddMember("sdsm_time_stamp", time_stamp_json, doc.GetAllocator());
        // Construct reference position JSON Object
        auto position_3d_json = create_position_3d( msg._ref_positon, doc.GetAllocator() );
        sdsm_json.AddMember("ref_pos", position_3d_json, doc.GetAllocator());
        sdsm_json.AddMember("ref_pos_xy_conf", create_positional_accuracy( msg._ref_position_confidence, doc.GetAllocator()), doc.GetAllocator());
        if ( msg._ref_position_elavation_confidence.has_value() )
            sdsm_json.AddMember("ref_pos_el_conf", static_cast<unsigned int >(msg._ref_position_elavation_confidence.value()), doc.GetAllocator());
        // Construct object list
        sdsm_json.AddMember("objects", create_detected_object_list(msg._objects, doc.GetAllocator()), doc.GetAllocator());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        sdsm_json.Accept(writer);
    
        return buffer.GetString();
    }

    rapidjson::Value create_timestamp(const time_stamp &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value time_stamp_json(rapidjson::kObjectType);
        time_stamp_json.AddMember("second", val.second, allocator);
        time_stamp_json.AddMember("minute", val.minute, allocator);
        time_stamp_json.AddMember("hour", val.hour, allocator);
        time_stamp_json.AddMember("day", val.day, allocator);
        time_stamp_json.AddMember("month", val.month, allocator);
        time_stamp_json.AddMember("year", val.year, allocator);
        time_stamp_json.AddMember("offset", val.offset, allocator);
        return time_stamp_json;
    }

    rapidjson::Value create_position_3d(const position_3d &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value position_3d_json(rapidjson::kObjectType);
        position_3d_json.AddMember("long", val._longitude, allocator);
        position_3d_json.AddMember("lat", val._latitude, allocator);
        if ( val._elavation.has_value() ) {
            position_3d_json.AddMember("elevation", val._elavation.value(), allocator);
        }
        return position_3d_json;
    }

    rapidjson::Value create_positional_accuracy(const positional_accuracy &val, rapidjson::Document::AllocatorType &allocator) {
        rapidjson::Value positional_accuracy_json(rapidjson::kObjectType);
        positional_accuracy_json.AddMember("semi_major", val._semi_major_axis_accuracy, allocator);
        positional_accuracy_json.AddMember("semi_minor", val._semi_minor_axis_accuracy, allocator);
        positional_accuracy_json.AddMember("orientation", val._semi_major_axis_orientation, allocator);
        return positional_accuracy_json;
    }
    rapidjson::Value create_detected_object_list(const std::vector<detected_object_data> &val, rapidjson::Document::AllocatorType &allocator ){
        rapidjson::Value detected_object_list_json(rapidjson::kArrayType);
        for (const auto &detected_obect : val) {
            // Create and push detected object data
            detected_object_list_json.PushBack(create_detected_object_data(detected_obect, allocator), allocator);
            
        }
        return detected_object_list_json;
    }

    rapidjson::Value create_detected_object_data(const detected_object_data &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value detected_object_data_json(rapidjson::kObjectType);
        // Create Common Data
        detected_object_data_json.AddMember("detected_object_common_data",create_detected_object_data_common(val._detected_object_common_data,allocator), allocator );
        // Create Optional Data
        if ( val._detected_object_optional_data.has_value() )
            detected_object_data_json.AddMember(
                    "detected_object_optional_data", 
                    create_detected_object_data_optional(val._detected_object_optional_data.value(), allocator), 
                    allocator);
        return detected_object_data_json;
    }


    rapidjson::Value create_detected_object_data_common(const detected_object_data_common &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value detected_object_data_common_json(rapidjson::kObjectType);
        detected_object_data_common_json.AddMember("obj_type", static_cast<unsigned int >(val._object_type), allocator);
        detected_object_data_common_json.AddMember("object_id", val._object_id, allocator);
        detected_object_data_common_json.AddMember("obj_type_cfd", val._classification_confidence, allocator);
        detected_object_data_common_json.AddMember("measurement_time", val._time_measurement_offset, allocator);
        detected_object_data_common_json.AddMember("time_confidence", static_cast<unsigned int >(val._time_confidence), allocator);
        // Create Position 
        detected_object_data_common_json.AddMember(
                    "pos", 
                    create_position_3d( val._position_offset, allocator), 
                    allocator);
        // Create Position Confidence
        detected_object_data_common_json.AddMember("pos_confidence", create_position_confidence_set(val._pos_confidence, allocator), allocator );
        detected_object_data_common_json.AddMember("speed", val._speed, allocator);
        detected_object_data_common_json.AddMember("speed_confidence", static_cast<unsigned int >(val._speed_confidence), allocator);
        if ( val._speed_z.has_value())
            detected_object_data_common_json.AddMember("speed_z", val._speed_z.value(), allocator);
        if ( val._speed_z_confidence.has_value())
            detected_object_data_common_json.AddMember(
                    "speed_confidence_z", 
                    static_cast<unsigned int >(val._speed_z_confidence.value()),
                    allocator);
        detected_object_data_common_json.AddMember("heading", val._heading, allocator);
        detected_object_data_common_json.AddMember("heading_conf", static_cast<unsigned int >(val._heading_confidence), allocator);
        if (val._lateral_acceleration_confidence.has_value() ) {
            detected_object_data_common_json.AddMember(
                    "acc_cfd_x", 
                    static_cast<unsigned int >(val._lateral_acceleration_confidence.value()),
                    allocator);
        if (val._longitudinal_acceleration_confidence.has_value() )
            detected_object_data_common_json.AddMember(
                    "acc_cfd_y", 
                    static_cast<unsigned int >(val._longitudinal_acceleration_confidence.value()),
                    allocator);
        if (val._vertical_accelaration_confidence.has_value())
            detected_object_data_common_json.AddMember(
                    "acc_cfd_z", 
                    static_cast<unsigned int >(val._vertical_accelaration_confidence.value()),
                    allocator);
        if (val._yaw_rate_confidence.has_value())
            detected_object_data_common_json.AddMember(
                    "acc_cfd_yaw", 
                    static_cast<unsigned int >(val._yaw_rate_confidence.value()),
                    allocator);
        }
        if (val._acceleration_4_way.has_value()) {
            // Create accel_4_way
            detected_object_data_common_json.AddMember(
                    "accel_4_way", 
                    create_accelaration_set_4_way(val._acceleration_4_way.value(), allocator), 
                    allocator);
        }
        return detected_object_data_common_json;
    }

    rapidjson::Value create_accelaration_set_4_way(const acceleration_set_4_way &val, rapidjson::Document::AllocatorType &allocator) {
        rapidjson::Value accelaration_set_4_way_json(rapidjson::kObjectType);
        accelaration_set_4_way_json.AddMember("lat", val._lateral_accel, allocator);
        accelaration_set_4_way_json.AddMember("long", val._longitudinal_accel, allocator);
        accelaration_set_4_way_json.AddMember("vert", val._vertical_accel, allocator);
        accelaration_set_4_way_json.AddMember("yaw", val._yaw_rate, allocator);
        return accelaration_set_4_way_json;
    }

    rapidjson::Value create_position_3d(const position_offset &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value position_3d_json(rapidjson::kObjectType);
        position_3d_json.AddMember("offset_x", val._offset_x, allocator);
        position_3d_json.AddMember("offset_y", val._offset_y, allocator);
        position_3d_json.AddMember("offset_z", val._offset_z, allocator);
        return position_3d_json;
    }

    rapidjson::Value create_position_confidence_set(const position_confidence_set &val, rapidjson::Document::AllocatorType &allocator) {
        rapidjson::Value position_confidence_json(rapidjson::kObjectType);
        position_confidence_json.AddMember("pos",static_cast<unsigned int >(val._position_confidence), allocator);
        position_confidence_json.AddMember("elavation", static_cast<unsigned int >(val._elavation_confidence), allocator);
        return position_confidence_json;
    }
    rapidjson::Value create_detected_object_data_optional(const std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data> &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value detected_object_data_optional_json(rapidjson::kObjectType);
        if (std::holds_alternative<detected_obstacle_data>(val)) {
            detected_object_data_optional_json.AddMember("detected_obstacle_data",create_detected_obstacle_data(std::get<detected_obstacle_data>(val),allocator), allocator);
            return detected_object_data_optional_json;
        }
        else if (std::holds_alternative<detected_vehicle_data>(val)) {
            detected_object_data_optional_json.AddMember("detected_vehicle_data",create_detected_vehicle_data(std::get<detected_vehicle_data>(val),allocator), allocator);
            return detected_object_data_optional_json;
        }
        else if (std::holds_alternative<detected_vru_data>(val)) {
            detected_object_data_optional_json.AddMember("detected_vru_data",create_detected_vru_data(std::get<detected_vru_data>(val),allocator), allocator);
            return detected_object_data_optional_json;
        }
        else {
            throw json_utils::json_parse_exception("If present, detected optional data must include one of the following objects : detected obstacle data, detected vehicle data, detected vru data"); 
        }
    }


    rapidjson::Value create_detected_obstacle_data(const detected_obstacle_data &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        data.AddMember("obst_size", create_obstacle_size(val._size, allocator), allocator);
        data.AddMember("obst_size_confidence", create_obstacle_size_confidence(val._size_confidence, allocator), allocator);
        return data;
    }

    rapidjson::Value create_obstacle_size(const obstacle_size &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        data.AddMember("width", val._width, allocator);
        data.AddMember("length", val._length, allocator);
        // Optional Height
        if (val._height.has_value() ) {
            data.AddMember("height", val._height.value(), allocator);
        }

        return data;
    }

    rapidjson::Value create_obstacle_size_confidence(const obstacle_size_confidence &val, rapidjson::Document::AllocatorType &allocator) {
        rapidjson::Value data(rapidjson::kObjectType);
        data.AddMember("width_confidence", static_cast<unsigned int>(val._width_confidence), allocator);
        data.AddMember("length_confidence", static_cast<unsigned int>(val._length_confidence), allocator);
        // Optional height
        if ( val._height_confidence.has_value()) {
            data.AddMember("height_confidence", static_cast<unsigned int>(val._height_confidence.value()), allocator);
        }
        return data;
    }

    rapidjson::Value create_detected_vru_data(const detected_vru_data &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        if (val._personal_device_user_type.has_value() ) {
            data.AddMember("basic_type", static_cast<unsigned int>(val._personal_device_user_type.value()), allocator);
        }
        if (val._attachment.has_value() ) {
            data.AddMember("attachment", static_cast<unsigned int>(val._attachment.value()), allocator);
        }
        if (val._propulsion.has_value() ) {
            data.AddMember("propulsion", create_propelled_information(val._propulsion.value(), allocator), allocator);
        }
        if (val._attachment_radius.has_value() ) {
            data.AddMember("radius", val._attachment_radius.value(), allocator);
        }
        return data;
    }

    rapidjson::Value create_detected_vehicle_data(const detected_vehicle_data &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        if ( val.exterior_lights.has_value() ) {
            data.AddMember("lights", val.exterior_lights.value(), allocator);
        }
        if ( val._veh_attitude.has_value() ) {
            data.AddMember("veh_attitude", create_vehicle_attitude(val._veh_attitude.value(), allocator), allocator);
        }
        if ( val._attitude_confidence.has_value() ) {
            data.AddMember("veh_attitude_confidence", create_vehicle_attitude_confidence(val._attitude_confidence.value(), allocator), allocator);
        }
        if ( val._angular_velocity.has_value() ) {
            data.AddMember("veh_ang_vel", create_angular_velocity(val._angular_velocity.value(), allocator), allocator);
        }
        if ( val._angular_velocity_confidence.has_value() ){
            data.AddMember("veh_ang_vel_confidence", create_angular_velocity_confidence(val._angular_velocity_confidence.value(),allocator), allocator);
        }
        if ( val._size.has_value() ) {
            data.AddMember("size", create_vehicle_size(val._size.value(), allocator), allocator);
        }
        if ( val._size_confidence.has_value() ) {
            data.AddMember("vehicle_size_confidence", create_vehicle_size_confidence(val._size_confidence.value(), allocator), allocator);
        }
        if (val._vehicle_height.has_value() ) {
            data.AddMember("height", val._vehicle_height.value(), allocator);
        }
        if (val._vehicle_class.has_value() ) {
            data.AddMember("vehicle_class", val._vehicle_class.value(), allocator);
        }
        if ( val._classification_confidence.has_value() ) {
            data.AddMember("vehicle_class_conf", val._classification_confidence.value(), allocator);
        }
        return data;
    }



    rapidjson::Value create_propelled_information(const std::variant<human_propelled_type, motorized_propelled_type,animal_propelled_type> &val, rapidjson::Document::AllocatorType &allocator) {
        rapidjson::Value data(rapidjson::kObjectType);
        if ( std::holds_alternative<human_propelled_type>(val) ) {
            data.AddMember("human", static_cast<unsigned int>( std::get<human_propelled_type>(val)), allocator);
            return data;
        }
        else if ( std::holds_alternative<animal_propelled_type>(val) ) {
            data.AddMember("animal", static_cast<unsigned int>( std::get<animal_propelled_type>(val)), allocator);
            return data;
        }
        else if ( std::holds_alternative<motorized_propelled_type>(val) ) {
            data.AddMember("motor", static_cast<unsigned int>( std::get<motorized_propelled_type>(val)), allocator);
            return data;
        }
        else {
            throw json_utils::json_parse_exception("If present, propelled infromation must include one of the following objects : animal propelled type, motorized propelled type, human propelled type"); 
        }
    }


    rapidjson::Value create_vehicle_attitude(const attitude &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        data.AddMember("pitch", val._pitch, allocator);
        data.AddMember("roll", val._roll, allocator);
        data.AddMember("yaw", val._yaw, allocator);
        return data;
    }

    rapidjson::Value create_vehicle_attitude_confidence(const attitude_confidence &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        data.AddMember("pitch_confidence", static_cast<unsigned int>(val._pitch_confidence), allocator);
        data.AddMember("roll_confidence", static_cast<unsigned int>(val._roll_confidence), allocator);
        data.AddMember("yaw_confidence", static_cast<unsigned int>(val._yaw_confidence), allocator);
        return data;
    }

    rapidjson::Value create_angular_velocity(const angular_velocity_set &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        data.AddMember("pitch_rate", static_cast<unsigned int>(val._pitch_rate), allocator);
        data.AddMember("roll_rate", static_cast<unsigned int>(val._roll_rate), allocator);
        return data;
    }

    rapidjson::Value create_angular_velocity_confidence(const angular_velocity_confidence_set &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        if ( val._pitch_rate_confidence.has_value() ) {
            data.AddMember("pitch_rate_confidence", static_cast<unsigned int>(val._pitch_rate_confidence.value()), allocator);
        }
        if ( val._roll_rate_confidence.has_value() ) {
            data.AddMember("roll_rate_confidence", static_cast<unsigned int>(val._roll_rate_confidence.value()), allocator);
        }
        return data;
    }

    rapidjson::Value create_vehicle_size(const vehicle_size &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        data.AddMember("width", val._width, allocator);
        data.AddMember("length", val._length, allocator);
        return data;
    }

    rapidjson::Value create_vehicle_size_confidence(const vehicle_size_confidence &val, rapidjson::Document::AllocatorType &allocator){
        rapidjson::Value data(rapidjson::kObjectType);
        data.AddMember("vehicle_width_confidence", static_cast<unsigned int>(val._width_confidence), allocator);
        data.AddMember("vehicle_length_confidence", static_cast<unsigned int>(val._length_confidence), allocator);
        if ( val._height_confidence.has_value() ) {
            data.AddMember("vehicle_height_confidence", static_cast<unsigned int>(val._height_confidence.value()), allocator);
        }
        return data;
    }


}