#include "deserializers/sensor_data_sharing_msg_json_deserializer.hpp"


namespace streets_utils::messages {
    using namespace streets_utils::json_utils;
    sensor_data_sharing_msg from_json( const std::string &json ){
        rapidjson::Document document = streets_utils::json_utils::parse_json(json);
        return sensor_data_sharing_msg();
    }    
}