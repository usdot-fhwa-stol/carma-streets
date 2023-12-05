#include "sensor_configuration_parser.hpp"

namespace sensor_data_sharing_service {

    lanelet::BasicPoint3d parse_sensor_location( const std::string &filepath , const std::string &sensor_id ){
        // Parse JSON configuration file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open Streets configuration file " + filepath + " !"); 
        }
        // Add file contents to stream and parse stream into Document
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document doc;
        doc.ParseStream(isw);
        if (doc.HasParseError()){
            SPDLOG_ERROR("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
            throw std::runtime_error("Document parse error!");
        }
        file.close();
        if (doc.IsArray()) {
            // Iterate over the array of objects
            rapidjson::Value::ConstValueIterator itr;
            for (itr = doc.Begin(); itr != doc.End(); ++itr) {
                // Access the data in the object
                auto _sensor_id = streets_utils::json_utils::parse_string_member("sensorId",  itr->GetObject(), true ).value();
                if ( _sensor_id == sensor_id ) {
                    auto location = streets_utils::json_utils::parse_object_member("location",  itr->GetObject(), true ).value();
                    return lanelet::BasicPoint3d{
                        streets_utils::json_utils::parse_double_member("x",  location, true ).value(),
                        streets_utils::json_utils::parse_double_member("y",  location, true ).value(),
                        streets_utils::json_utils::parse_double_member("z",  location, true ).value()
                        };
                }
            }
        }
       
    }
}