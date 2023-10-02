#pragma once 
#include <stdlib.h>
#include <string>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <optional>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <spdlog/spdlog.h>
#include "json_utils_exception.hpp"

namespace streets_utils::json_utils {

    /**
     * @brief Function to parse `std::string` JSON with [RapidJSON](https://miloyip.github.io/rapidjson/index.html) with 
     * [DOM  parsing](https://miloyip.github.io/rapidjson/md_obj_dom.html). Throws `json_utils_exception` if string is 
     * not valid json. Otherwise returns `rapidjson::Document`
     * @param json std::string JSON to parse
     * @throws json_utils_exception if passed string is not valid json.
     * @return resulting `rapidjson::Document`
     */
    rapidjson::Document validate_json( const std::string &json );
    /**
     * @brief Functions to retrieve int member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_utils_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_utils_exception if member is required but not found.
     * @return std::optional<int64_t> 
     */
    std::optional<int64_t> parse_int_member(const std::string &member_name, const rapidjson::Value &obj, bool required );
    /**
     * @brief Functions to retrieve uint member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_utils_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_utils_exception if member is required but not found.
     * @return std::optional<uint64_t> 
     */
    std::optional<uint64_t> parse_uint_member(const std::string &member_name,  const rapidjson::Value &obj, bool required );
     /**
     * @brief Functions to retrieve bool member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_utils_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_utils_exception if member is required but not found.
     * @return std::optional<boll> 
     */
    std::optional<bool> parse_bool_member(const std::string &member_name,  const rapidjson::Value &obj, bool required );
    /**
     * @brief Functions to retrieve std::string member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_utils_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_utils_exception if member is required but not found.
     * @return std::optional<std::string> 
     */
    std::optional<std::string> parse_string_member(const std::string &member_name,  const rapidjson::Value &obj, bool required );
    /**
     * @brief Functions to retrieve double member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_utils_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_utils_exception if member is required but not found.
     * @return std::optional<double> 
     */
    std::optional<double> parse_double_member(const std::string &member_name, const rapidjson::Value &obj, bool required);
    /**
     * @brief Functions to retrieve object member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_utils_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_utils_exception if member is required but not found.
     * @return std::optional<rapidJson::Value::ConstObject> 
     */
    std::optional<rapidjson::Value::ConstObject> parse_object_member(const std::string &member_name, const rapidjson::Value &obj, bool required );
    /**
     * @brief Functions to retrieve array member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html)
     * with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized
     * `std::optional` for optional members that are not found in JSON object and will throw `json_utils_exception`. 
     * for required members that are not found in JSON object.
     * @param member_name string member name to search for.
     * @param obj JSON object with member
     * @param required bool flag to indicate whether member is required by calling code.
     * @throws json_utils_exception if member is required but not found.
     * @return std::optional<rapidjson::Value::ConstArray> 
     */
    std::optional<rapidjson::Value::ConstArray> parse_array_member(const std::string &member_name, const rapidjson::Value &obj, bool required);
    // TODO: Commented out because I can not get this to work for string values. Before closing PR make a decision on keeping/fixing this or removing
    // it. Works for all other types other 
    // template<typename T>
    // inline void write_optional_member(const std::string &member_name, const std::optional<T> _value, rapidjson::Value &obj, rapidjson::Document::AllocatorType &allocator)
    // {
    //     if (obj.IsObject() && _value.has_value()) {
    //         // In an attempt to improve performance and since many json objects have constant
    //         // key names the StringRef constructor avoids copying the member name and "requires 
    //         // that the referenced string pointers have a sufficient lifetime, which exceeds the 
    //         // lifetime of the associated GenericValue"(https://rapidjson.org/structrapidjson_1_1_generic_string_ref.html). 

    //         // To explicitly force a copy we instead create a new String Value with the content of 
    //         // the passed in parameter. Follow link to get more information from Documentation 
    //         // (RapidJSON Documentation)[https://miloyip.github.io/rapidjson/md_obj_tutorial.html#ModifyObject]

    //         // Alternatively we could pass in the string reference using StringRef(member_name). Doing so
    //         // we would be explicitly guaranteeing the lifetime of the passed in string reference. If we then
    //         // pass in a string literal from the caller of this function the AddMember method would silently fail
    //         // since the lifetime of the reference string is not sufficient. 
    //         rapidjson::Value member_name_copy(member_name, allocator);
    //         obj.AddMember(member_name_copy, _value.value(), allocator);
    //   
    //     }
    //     else {
    //         SPDLOG_TRACE("Skipping member {0}! std::optional has_value() check failed or parameter is not writable object type value!", 
    //                         member_name);

    //     }
    // };

    // template<typename T>
    // inline void write_required_member(const std::string &member_name, const T &value, rapidjson::Value &obj, rapidjson::Document::AllocatorType &allocator){
    //     if ( obj.IsObject() && value != NULL ) {
    //         rapidjson::Value member_name_copy(member_name, allocator);
    //         obj.AddMember(member_name_copy, value, allocator);
    //     }
    //     else {
    //         throw json_utils_exception("");
    //     }
    // };

    
}