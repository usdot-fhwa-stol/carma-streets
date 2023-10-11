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
            data._detected_object_common_data = parse_detected_object_data_common(parse_object_member("detected_object_common_data", val, true).value());
            if ( auto data_optional = parse_object_member("detected_object_optional_data", val, false); data_optional.has_value()) {
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
        return _detected_object_common_data;
    }

    std::optional<std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data>>  parse_detected_object_data_optional(const rapidjson::Value &val){
        std::optional<std::variant<detected_obstacle_data, detected_vehicle_data, detected_vru_data>> detected_optional_data;
        if (val.HasMember("detected_vehicle_data")){
            detected_vehicle_data data;
            detected_optional_data = data;
        }
        else if (val.HasMember("detected_vru_data")) {

        }
        else if (val.HasMember("detected_obstacle_data") ) {

        }
        return detected_optional_data;
    }
}