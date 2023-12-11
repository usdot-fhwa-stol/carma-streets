#include "json_utils.hpp"

namespace streets_utils::json_utils {

    rapidjson::Document parse_json(const std::string &json) {
        rapidjson::Document obj;
        obj.Parse(json);
        if (obj.HasParseError())
        {
            // TODO: Change to json_document_parse_exception. Requires changes to services and unit tests
            throw json_parse_exception("Message JSON is misformatted. JSON parsing failed!");
        }
        return obj;
    }

    rapidjson::Document parse_json_file(const std::string &filepath) {
         // Parse JSON configuration file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open Streets configuration file " + filepath + " !"); 
        }
        // Add file contents to stream and parse stream into Document
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document doc;
        doc.ParseStream(isw);
        if (doc.HasParseError()){
            throw json_document_parse_error("Encounter document parse error while attempting to parse sensor configuration file " + filepath + "!", doc);
        }
        file.close();
        return doc;
    }
    std::optional<int> parse_int_member(const std::string &member_name, const rapidjson::Value &obj, bool required ){
        std::optional<int64_t> member;
        if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsInt64())
        {
            member = obj[member_name.c_str()].GetInt64();
        }
        else if (required)
        {
            throw json_parse_exception("Missing or incorrect type for required member " + member_name + "!");
        }
        return member;
    }

    std::optional<unsigned int> parse_uint_member(const std::string &member_name,  const rapidjson::Value &obj, bool required ) {
        std::optional<u_int64_t> member;
        if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsUint64())
        {
            member = obj[member_name.c_str()].GetUint64();
        }
        else if (required)
        {
            throw json_parse_exception("Missing or incorrect type for required member " + member_name + "!");
        }
        return member;
    };

    std::optional<bool> parse_bool_member(const std::string &member_name,  const rapidjson::Value &obj, bool required ) {
        std::optional<bool> member;
        if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsBool())
        {
            member = obj[member_name.c_str()].GetBool();
        }
        else if (required)
        {
            throw json_parse_exception("Missing or incorrect type for required member " + member_name + "!");
        }
        return member;
    };

    std::optional<std::string> parse_string_member(const std::string &member_name,  const rapidjson::Value &obj, bool required ){
        std::optional<std::string> member;
        if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsString())
        {
            member = obj[member_name.c_str()].GetString();
        }
        else if (required)
        {
            throw json_parse_exception("Missing or incorrect type for required member " + member_name + "!");
        }
        return member;
    };

    std::optional<double> parse_double_member(const std::string &member_name, const rapidjson::Value &obj, bool required) {
        std::optional<double> member;
        if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsDouble())
        {
            member = obj[member_name.c_str()].GetDouble();
        }
        else if (required)
        {
            throw json_parse_exception("Missing or incorrect type for required member " + member_name + "!");
        }
        return member;
    };

    std::optional<rapidjson::Value::ConstObject> parse_object_member(const std::string &member_name, const rapidjson::Value &obj, bool required) {
        if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsObject())
        {
            return obj[member_name.c_str()].GetObject();
        }
        else if (required)
        {
            throw json_parse_exception("Missing or incorrect type for required member " + member_name + "!");
        }
        return std::nullopt;
    }

    std::optional<rapidjson::Value::ConstArray> parse_array_member(const std::string &member_name, const rapidjson::Value &obj, bool required) {
        if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsArray())
        {
            return obj[member_name.c_str()].GetArray();
        }
        else if (required)
        {
            throw json_parse_exception("Missing or incorrect type for required member " + member_name + "!");
        }
        return std::nullopt;
    }



}