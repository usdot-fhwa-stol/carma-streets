#pragma once 

#include <stdlib.h>
#include <string>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <optional>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "json_parsing_exception.hpp"

namespace streets_utils::json_utils {

    rapidjson::Document validate_json( const std::string &json );

    std::optional<int64_t> parse_int_property(const std::string &prop_name, const rapidjson::Value &doc, bool required );

    std::optional<uint64_t> parse_uint_property(const std::string &prop_name,  const rapidjson::Value &doc, bool required );

    std::optional<bool> parse_bool_property(const std::string &prop_name,  const rapidjson::Value &doc, bool required );

    std::optional<std::string> parse_string_property(const std::string &prop_name,  const rapidjson::Value &doc, bool required );

    std::optional<double> parse_double_property(const std::string &prop_name, const rapidjson::Value &doc, bool required);

    std::optional<rapidjson::Value::ConstObject> parse_object_property(const std::string &prop_name, const rapidjson::Value &doc, bool required );

    std::optional<rapidjson::Value::ConstArray> parse_array_property(const std::string &prop_name, const rapidjson::Value &doc, bool required);

    template<typename T>
    inline void write_optional_property(const std::string &prop_name, std::optional<T> value, rapidjson::Value &doc, rapidjson::Document::AllocatorType &allocator)
    {
        if (doc.IsObject() && value->has_value()) {
            // Add Member
            doc.AddMember(prop_name, value, allocator);
        }
    };
    
    template<typename T>
    inline void write_required_property(const std::string &prop_name, T value, rapidjson::Value &doc, rapidjson::Document::AllocatorType &allocator){
        doc.AddMember(prop_name, value, allocator);
    };

    
}