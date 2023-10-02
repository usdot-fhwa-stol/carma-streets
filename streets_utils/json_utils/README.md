# JSON Utility Library

## Introduction

This CARMA-Streets library contains utility functions for parsing JSON strings with [RapidJSON](https://miloyip.github.io/rapidjson/index.html). The functions exposed by this library are described below

## Functions
`rapidjson::Document parse_json( const std::string &json )`

Function to parse `std::string` JSON with [RapidJSON](https://miloyip.github.io/rapidjson/index.html) with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Throws `json_utils_exception` if string is not valid json. Otherwise returns `rapidjson::Document``

`std::optional<int64_t> parse_int_member(const std::string &member_name, const rapidjson::Value &doc, bool required )`
`std::optional<uint64_t> parse_uint_member(const std::string &member_name,  const rapidjson::Value &doc, bool required )`
`std::optional<bool> parse_bool_member(const std::string &member_name,  const rapidjson::Value &doc, bool required )`
`std::optional<std::string> parse_string_member(const std::string &member_name,  const rapidjson::Value &doc, bool required )`
`std::optional<double> parse_double_member(const std::string &member_name, const rapidjson::Value &doc, bool required)`
`std::optional<rapidjson::Value::ConstObject> parse_object_member(const std::string &member_name, const rapidjson::Value &doc, bool required )`
`std::optional<rapidjson::Value::ConstArray> parse_array_member(const std::string &member_name, const rapidjson::Value &doc, bool required)`

Functions to retrieve member values from JSON object [RapidJSON](https://miloyip.github.io/rapidjson/index.html) with [DOM  parsing](https://miloyip.github.io/rapidjson/md_doc_dom.html). Functions will return unintialized `std::optional` for optional members that are not found in JSON object and will throw `json_utils_exception` for required members that are not found in JSON object.

## Include Library
To include after installing with `make install` simply add find_package call to `CMakeLists.txt` and link the library as follows. Installed CMake configuration files should handle finding and linking depedencies `RapidJSON` and `spdlog`. The library along with it's dependencies can then be included by simply using the find_package() instruction.
```
find_package(json_utils_lib)
...
target_link_library( target PUBLIC streets_utils::json_utils)
```