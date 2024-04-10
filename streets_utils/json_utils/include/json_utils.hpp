#pragma once 
#include <stdlib.h>
#include <string>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <optional>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <spdlog/spdlog.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>
#include "json_utils_exception.hpp"
#include "json_document_parse_error.hpp"

namespace streets_utils::json_utils {

    /**
     * @brief Function to parse `std::string` JSON with [RapidJSON](https://miloyip.github.io/rapidjson/index.html) with 
     * [DOM  parsing](https://miloyip.github.io/rapidjson/md_obj_dom.html). Throws `json_parse_exception` if string is 
     * not valid json. Otherwise returns `rapidjson::Document`
     * @param json std::string JSON to parse
     * @throws json_parse_exception if passed string is not valid json.
     * @return resulting `rapidjson::Document`
     */
    rapidjson::Document parse_json( const std::string &json );
/**
     * @brief Function to parse file containing JSON with [RapidJSON](https://miloyip.github.io/rapidjson/index.html) with 
     * [DOM  parsing](https://miloyip.github.io/rapidjson/md_obj_dom.html). Throws `json_parse_exception` if string is 
     * not valid json. Otherwise returns `rapidjson::Document`
     * @param json std::string JSON to parse
     * @throws json_parse_exception if passed string is not valid json.
     * @return resulting `rapidjson::Document`
     */
    rapidjson::Document parse_json_file(const std::string &filepath);
    /**
     * @brief Functions to retrieve int member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_parse_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_parse_exception if member is required but not found.
     * @return std::optional<int> 
     */
    std::optional<int> parse_int_member(const std::string &member_name, const rapidjson::Value &obj, bool required );
    /**
     * @brief Functions to retrieve uint member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_parse_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_parse_exception if member is required but not found.
     * @return std::optional<unsigned int> 
     */
    std::optional<unsigned int> parse_uint_member(const std::string &member_name,  const rapidjson::Value &obj, bool required );
     /**
     * @brief Functions to retrieve bool member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_parse_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_parse_exception if member is required but not found.
     * @return std::optional<boll> 
     */
    std::optional<bool> parse_bool_member(const std::string &member_name,  const rapidjson::Value &obj, bool required );
    /**
     * @brief Functions to retrieve std::string member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_parse_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_parse_exception if member is required but not found.
     * @return std::optional<std::string> 
     */
    std::optional<std::string> parse_string_member(const std::string &member_name,  const rapidjson::Value &obj, bool required );
    /**
     * @brief Functions to retrieve double member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_parse_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_parse_exception if member is required but not found.
     * @return std::optional<double> 
     */
    std::optional<double> parse_double_member(const std::string &member_name, const rapidjson::Value &obj, bool required);
    /**
     * @brief Functions to retrieve object member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_parse_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_parse_exception if member is required but not found.
     * @return std::optional<rapidJson::Value::ConstObject> 
     */
    std::optional<rapidjson::Value::ConstObject> parse_object_member(const std::string &member_name, const rapidjson::Value &obj, bool required );
    /**
     * @brief Functions to retrieve array member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_parse_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_parse_exception if member is required but not found.
     * @return std::optional<rapidjson::Value::ConstArray> 
     */
    std::optional<rapidjson::Value::ConstArray> parse_array_member(const std::string &member_name, const rapidjson::Value &obj, bool required);
    

    
}