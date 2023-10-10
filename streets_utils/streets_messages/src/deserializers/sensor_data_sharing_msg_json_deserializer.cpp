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

        if (val.IsObject()) {
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
        else {
            throw json_parse_exception("Value passed is not a json object!");
        }
    }

    position_3d parse_position_3d(const rapidjson::Value &val) {
        if (val.IsObject()) {
            position_3d _position_3d;
            _position_3d._longitude = parse_int_member("long", val, true).value();
            _position_3d._latitude = parse_int_member("lat", val, true).value();
            // Optional
            _position_3d._elavation = parse_int_member("elevation", val, false);
            return _position_3d;
        }
        else {
            throw json_parse_exception("Value passed is not a json object!");
        }
    }

    positional_accuracy parse_positional_accuracy(const rapidjson::Value &val){
        if (val.IsObject()) {
            positional_accuracy _positional_accuracy;
            _positional_accuracy._semi_major_axis_accuracy = parse_uint_member("semi_major", val, true).value();
            _positional_accuracy._semi_minor_axis_accuracy = parse_uint_member("semi_minor", val, true).value();
            _positional_accuracy._semi_major_axis_orientation = parse_uint_member("orientation",val, true).value();
            return _positional_accuracy;
        }
        else {
            throw json_parse_exception("Value passed is not a json object!");
        }
    }

    std::optional<position_confidence> parse_elevation_confidence(const rapidjson::Value &val) {
        std::optional<position_confidence> _position_confidence;
        if (val.IsObject()) {
            auto _position_confidence_value = parse_uint_member("ref_pos_el_conf", val, false);
            if ( _position_confidence_value.has_value()) {
                _position_confidence = position_confidence_from_int(_position_confidence_value.value());
            }
        }
        return _position_confidence;   
    }
}