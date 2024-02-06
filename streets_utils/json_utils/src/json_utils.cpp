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
            throw std::invalid_argument("Unable to open Streets configuration file " + filepath + " due to : " + strerror(errno) + "."); 
        }
        // Add file contents to stream and parse stream into Document
        rapidjson::IStreamWrapper isw(file);
        rapidjson::Document doc;
        doc.ParseStream<rapidjson::ParseFlag::kParseNumbersAsStringsFlag>(isw);

        // doc.ParseStream<rapidjson::ParseFlag::kParseNumbersAsStringsFlag>(isw);
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
        else if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsString()) {
            auto string_int = obj[member_name.c_str()].GetString();
            try {
                member = std::stoi(string_int);
            }
            catch(const std::invalid_argument &e) {
                throw json_parse_exception("Incorrect type for required int member " + member_name + ": " + e.what());
            }
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
        else if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsString()) {
            auto string_int = obj[member_name.c_str()].GetString();
            try {
                member = std::stoi(string_int);
            }
            catch(const std::invalid_argument &e) {
                throw json_parse_exception("Incorrect type for required uint member " + member_name + ": " + e.what());
            }
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
        else if (obj.HasMember(member_name.c_str()) && obj.FindMember(member_name.c_str())->value.IsString()) {
            auto string_double = obj[member_name.c_str()].GetString();
            try {
                member = std::stod(string_double);
            }
            catch(const std::invalid_argument &e) {
                throw json_parse_exception("Incorrect type for required double member " + member_name + ": " + e.what());
            }
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
    // std::vector<std::vector<double>> parse_matrix_double(const rapidjson::Value::ConstArray &val)
    // {
    //     // Initialize 3x3 covariance matrix
    //     std::vector<std::vector<double>> covariance;

    //     for (rapidjson::SizeType i = 0; i < val.Size(); i++)
    //     {
    //         const rapidjson::Value& row = val[i];
    //         std::vector<double> val_row;

    //         for (rapidjson::SizeType  j = 0; j < row.Size(); j++)
    //         {
    //             double member;
    //             if (val[i][j].IsDouble())
    //             {
    //                 member = val[i][j].GetDouble();
    //             }
    //             else if (val[i][j].IsString()) {
    //                 auto string_double = val[i][j].GetString();
    //                 try {
    //                     member = std::stod(string_double);
    //                 }
    //                 catch(const std::invalid_argument &e) {
    //                     throw std::runtime_error(e.what());
    //                 }
    //             }
    //             val_row.push_back(member);
    //         }
    //         covariance.push_back(val_row);
    //     }
    //     return covariance;
    // }



}