#pragma once 

#include <stdlib.h>
#include <string>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <optional>
#include "json_parsing_exception.hpp"

namespace streets_utils::json_utils {

    rapidjson::Document validate_json( const std::string &json );

    std::optional<int64_t> get_json_int_property(const std::string &prop_name, const rapidjson::Value &doc, bool required );

    std::optional<uint64_t> get_json_uint_property(const std::string &prop_name,  const rapidjson::Value &doc, bool required );

    std::optional<bool> get_json_bool_property(const std::string &prop_name,  const rapidjson::Value &doc, bool required );

    std::optional<std::string> get_json_string_property(const std::string &prop_name,  const rapidjson::Value &doc, bool required );

    std::optional<double> get_json_double_property(const std::string &prop_name, const rapidjson::Value &doc, bool required);

    rapidjson::Value::ConstObject get_json_object_property(const std::string &prop_name, const rapidjson::Value &doc, bool required );

    rapidjson::Value::ConstArray get_json_array_property(const std::string &prop_name, const rapidjson::Value &doc, bool required);

    void write_int_property(const std::string &prop_name, rapidjson::Value &doc);

    void write_uint_propert(const std::string &prop_name, rapidjson::Value &doc);

    
}