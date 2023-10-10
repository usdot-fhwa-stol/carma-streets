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
}