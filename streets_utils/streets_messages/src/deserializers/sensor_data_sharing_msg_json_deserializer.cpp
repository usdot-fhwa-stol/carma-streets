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
#include "deserializers/sensor_data_sharing_msg_json_deserializer.hpp"


namespace streets_utils::messages {
    using namespace streets_utils::json_utils;
    sensor_data_sharing_msg from_json( const std::string &json ){
        rapidjson::Document document = streets_utils::json_utils::parse_json(json);
        sensor_data_sharing_msg msg;
        msg._msg_count = parse_uint_member("msg_cnt", document, true).value();
        msg._source_id = parse_string_member("source_id", document, true).value();
        msg._equipment_type = equipment_type_from_int(parse_uint_member("equipment_type", document, true).value());
        msg._time_stamp = parse_time_stamp(parse_object_member("sdsm_time_stamp", document, true).value());
        msg._ref_positon = parse_position_3d(parse_object_member("ref_pos", document, true).value());
        msg._ref_position_confidence = parse_positional_accuracy(parse_object_member("ref_pos_xy_conf", document, true).value());
        msg._ref_position_elavation_confidence = parse_elevation_confidence( document);
        msg._objects = parse_detected_object_list(document);
        return msg;
    }  

    time_stamp parse_time_stamp(const rapidjson::Value &val){
        time_stamp _time_stamp;
        _time_stamp.offset = parse_int_member("offset", val, true).value();
        _time_stamp.second = parse_uint_member("second", val, true).value();
        _time_stamp.minute = parse_uint_member("minute", val, true).value();
        _time_stamp.hour = parse_uint_member("hour", val, true).value();
        _time_stamp.day = parse_uint_member("day", val, true).value();
        _time_stamp.month = parse_uint_member("month", val, true).value();
        _time_stamp.year = parse_uint_member("year", val, true).value();
        return _time_stamp;
        
    }

    position_3d parse_position_3d(const rapidjson::Value &val) {
        position_3d _position_3d;
        _position_3d._longitude = parse_int_member("long", val, true).value();
        _position_3d._latitude = parse_int_member("lat", val, true).value();
        // parse optional elevation 
        _position_3d._elavation = parse_int_member("elevation", val, false);
        return _position_3d;

    }

    positional_accuracy parse_positional_accuracy(const rapidjson::Value &val){
        positional_accuracy _positional_accuracy;
        _positional_accuracy._semi_major_axis_accuracy = parse_uint_member("semi_major", val, true).value();
        _positional_accuracy._semi_minor_axis_accuracy = parse_uint_member("semi_minor", val, true).value();
        _positional_accuracy._semi_major_axis_orientation = parse_uint_member("orientation",val, true).value();
        return _positional_accuracy;
    
    }

    std::optional<position_confidence> parse_elevation_confidence(const rapidjson::Value &val) {
        std::optional<position_confidence> _position_confidence;
        if (auto _position_confidence_value = parse_uint_member("ref_pos_el_conf", val, false); _position_confidence_value.has_value() ) {
            _position_confidence = position_confidence_from_int(_position_confidence_value.value());
        }
        return _position_confidence;   
    }

    std::vector<detected_object_data> parse_detected_object_list(const rapidjson::Value &val){
        std::vector<detected_object_data> detected_object_list;
        auto json_detected_object_list = parse_array_member("objects",val, true).value();
        for (const auto &object: json_detected_object_list){
            detected_object_data data;
            auto detected_object = parse_object_member("detected_object_data",object, true);
            data._detected_object_common_data = parse_detected_object_data_common(parse_object_member("detected_object_common_data", detected_object.value(), true).value());
            if ( auto data_optional = parse_object_member("detected_object_optional_data", detected_object.value(), false); data_optional.has_value()) {
                data._detected_object_optional_data = parse_detected_object_data_optional( data_optional.value() );
            }
            detected_object_list.push_back(data);
        }
        return detected_object_list;
    }

    detected_object_data_common parse_detected_object_data_common(const rapidjson::Value &val){
        detected_object_data_common _detected_object_common_data;
        _detected_object_common_data._object_type = object_type_from_int(parse_uint_member("obj_type", val, true).value());
        _detected_object_common_data._classification_confidence = parse_uint_member("obj_type_cfd",val,true).value();
        _detected_object_common_data._object_id = parse_uint_member("object_id", val, true).value();
        _detected_object_common_data._time_measurement_offset = parse_int_member("measurement_time", val, true).value();
        _detected_object_common_data._time_confidence = time_confidence_from_int(parse_uint_member("time_confidence", val, true).value());
        _detected_object_common_data._acceleration_4_way = parse_acceleration_4_way(parse_object_member("accel_4_way", val, true ).value());
        if ( val.HasMember("acc_cfd_x")) {
            _detected_object_common_data._lateral_acceleration_confidence = acceleration_confidence_from_int(parse_uint_member("acc_cfd_x", val, true).value()); 
        }
        if ( val.HasMember("acc_cfd_y")) {
            _detected_object_common_data._longitudinal_acceleration_confidence = acceleration_confidence_from_int(parse_uint_member("acc_cfd_y", val, true).value()); 
        }
        if ( val.HasMember("acc_cfd_z")) {
            _detected_object_common_data._vertical_accelaration_confidence = acceleration_confidence_from_int(parse_uint_member("acc_cfd_z", val, true).value()); 
        }
        if ( val.HasMember("acc_cfd_yaw")) {
            _detected_object_common_data._yaw_rate_confidence = angular_velocity_confidence_from_int(parse_uint_member("acc_cfd_yaw", val, true).value()); 
        }
        return _detected_object_common_data;
    }

    std::optional<std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data>>  parse_detected_object_data_optional(const rapidjson::Value &val){
        std::optional<std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data>> detected_optional_data;
        if (val.HasMember("detected_vehicle_data")){
            detected_optional_data = parse_detected_vehicle_data(parse_object_member("detected_vehicle_data", val, true).value());
        }
        else if (val.HasMember("detected_vru_data")) {
            detected_optional_data = parse_detected_vru_data(parse_object_member("detected_vru_data", val, true).value());

        }
        else if (val.HasMember("detected_obstacle_data") ) {
            detected_optional_data = parse_detected_obstacle_data(parse_object_member("detected_obstacle_data", val, true).value());
        }
        return detected_optional_data;
    }

    acceleration_set_4_way parse_acceleration_4_way(const rapidjson::Value &val) {
        acceleration_set_4_way data;
        data._lateral_accel = parse_int_member("lat", val, true).value();
        data._longitudinal_accel = parse_int_member("long", val, true).value();
        data._vertical_accel = parse_int_member("vert", val, true).value();
        data._yaw_rate = parse_int_member("yaw", val, true).value();
        return data;
    }

    detected_obstacle_data parse_detected_obstacle_data(const rapidjson::Value &val){
        detected_obstacle_data data;
        data._size = parse_obstacle_size(parse_object_member("obst_size", val, true).value());
        data._size_confidence = parse_obstacle_size_confidence(parse_object_member("obst_size_confidence", val, true).value());
        return data;
    }

    detected_vehicle_data parse_detected_vehicle_data(const rapidjson::Value &val){
        detected_vehicle_data data;
        data.exterior_lights = parse_string_member("lights", val, false); {
        if ( val.HasMember("veh_attitude"))
            data._veh_attitude = parse_vehicle_attitude(parse_object_member("veh_attitude", val, false).value());
        }
        if ( val.HasMember("veh_attitude_confidence")) {
            data._attitude_confidence = parse_vehicle_attitude_confidence(parse_object_member("veh_attitude_confidence", val, false).value());
        }
        if ( val.HasMember("veh_ang_vel")) {
            data._angular_velocity = parse_vehicle_angular_velocity_set(parse_object_member("veh_ang_vel", val, false).value());
        }
        if ( val.HasMember("veh_ang_vel_confidence")) {
            data._angular_velocity_confidence = parse_vehicle_angular_velocity_confidence_set(parse_object_member("veh_ang_vel_confidence", val, false).value());
        }
        if ( val.HasMember("size")) {
            data._size = parse_vehicle_size(parse_object_member("size", val, false).value());
        }
        if ( val.HasMember("vehicle_size_confidence")) {
            data._size_confidence = parse_vehicle_size_confidence(parse_object_member("vehicle_size_confidence", val, false).value());
        }
        data._vehicle_height = parse_uint_member("height", val, false);
        data._vehicle_class = parse_uint_member("vehicle_class", val, false);
        data._classification_confidence =  parse_uint_member("class_conf", val, false);
        return data;
    }


    attitude_confidence parse_vehicle_attitude_confidence(const rapidjson::Value &val){
        attitude_confidence data;
        data._pitch_confidence = heading_confidence_from_int(parse_uint_member("pitch_confidence", val, true).value());
        data._roll_confidence = heading_confidence_from_int(parse_uint_member("roll_confidence", val, true).value());
        data._yaw_confidence = heading_confidence_from_int(parse_uint_member("yaw_confidence", val, true).value());
        return data;

    }

    angular_velocity_set parse_vehicle_angular_velocity_set(const rapidjson::Value &val){
        angular_velocity_set data;
        data._pitch_rate = parse_int_member("pitch_rate", val, true).value();
        data._roll_rate = parse_int_member("roll_rate", val, true).value();
        return data;

    }

    angular_velocity_confidence_set parse_vehicle_angular_velocity_confidence_set(const rapidjson::Value &val){
        angular_velocity_confidence_set data;
        data._pitch_rate_confidence = angular_velocity_confidence_from_int( parse_uint_member("pitch_rate_confidence", val, true).value());
        data._roll_rate_confidence = angular_velocity_confidence_from_int( parse_uint_member("roll_rate_confidence", val, true).value());
        return data;
    }

    vehicle_size parse_vehicle_size(const rapidjson::Value &val){
        vehicle_size data;
        data._length = parse_uint_member("length", val, true).value();
        data._width = parse_uint_member("width", val, true).value();
        return data;
    }

    vehicle_size_confidence parse_vehicle_size_confidence(const rapidjson::Value &val){
        vehicle_size_confidence data;
        data._width_confidence = size_value_confidence_from_int(parse_uint_member("vehicle_width_confidence", val, true).value());
        data._length_confidence = size_value_confidence_from_int(parse_uint_member("vehicle_length_confidence", val, true).value());
        if (auto height_confidence_val = parse_uint_member("vehicle_height_confidence", val, false); height_confidence_val.has_value()) {
            data._height_confidence = size_value_confidence(height_confidence_val.value());
        }
        return data;
    }

    attitude parse_vehicle_attitude(const rapidjson::Value &val) {
        attitude data;
        data._pitch = parse_int_member("pitch", val, true).value();
        data._roll = parse_int_member("roll", val, true).value();
        data._yaw = parse_int_member("yaw", val, true).value();
        return data;
    }

    detected_vru_data parse_detected_vru_data(const rapidjson::Value &val){
        detected_vru_data data;
        if ( auto basic_type_value = parse_uint_member("basic_type", val, false); basic_type_value.has_value() ) {
            data._personal_device_user_type = personal_device_user_type_from_int( basic_type_value.value());
        }
        if ( val.HasMember("propulsion") ) {
            data._propulsion = parse_propelled_information(parse_object_member("propulsion", val, true).value());
        }
        if (auto attachment_value = parse_uint_member("attachment", val, false); attachment_value.has_value()  ){
            data._attachment = attachment_from_int( attachment_value.value());
        }
        data._attachment_radius = parse_uint_member("radius", val, false);
        return data;
    }

    std::optional<std::variant<human_propelled_type, motorized_propelled_type, animal_propelled_type>> parse_propelled_information( const rapidjson::Value &val){
        std::optional<std::variant<human_propelled_type, motorized_propelled_type, animal_propelled_type>> data;
        if (val.HasMember("human")) {
            data = human_propelled_type_from_int(parse_uint_member("human", val, true).value());
        }
        else if (val.HasMember("animal")) {
            data = animal_propelled_type_from_int(parse_uint_member("animal", val, true).value());        
        }
        else if (val.HasMember("motor")) {
            data = motorized_propelled_type_from_int(parse_uint_member("motor", val, true).value());
        }
        else {
            throw json_parse_exception("Propelled information requires one of the following to be define: human propelled type, animal propelled type, motorized propelled type.");
        }
        return data;
    } 


    obstacle_size parse_obstacle_size(const rapidjson::Value &val) {
        obstacle_size data;
        data._length = parse_uint_member("length", val, true).value();
        data._width = parse_uint_member("width", val, true).value();
        // Optional height
        data._height = parse_uint_member("height", val, true);
        return data;
    }

    obstacle_size_confidence parse_obstacle_size_confidence(const rapidjson::Value &val) {
        obstacle_size_confidence data;
        data._length_confidence = size_value_confidence_from_int( parse_uint_member("length_confidence", val, true).value());
        data._width_confidence = size_value_confidence_from_int( parse_uint_member("width_confidence", val, true).value());
        // Optional height value
        if ( auto height_confidence_value = parse_uint_member("height_confidence", val, false); height_confidence_value.has_value() ){
            data._height_confidence = size_value_confidence_from_int( parse_uint_member("height_confidence", val, true).value());
        }
        return data;
    }
}