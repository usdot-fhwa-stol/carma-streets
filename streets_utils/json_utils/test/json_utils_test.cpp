#include <gtest/gtest.h>
#include <json_utils.hpp>
#include <json_utils_exception.hpp>

using namespace streets_utils::json_utils;

//---------------------test parse_json---------------------
TEST(JsonUtilsTest, testParseJsonInvalidJson) {
    // Empty String
    std::string invalid_json = "";
    EXPECT_THROW( parse_json(invalid_json), json_parse_exception);

    // Property missing quotations
    invalid_json = "{ some_propert: \"some_value\"}";
    EXPECT_THROW( parse_json(invalid_json), json_parse_exception);

}

TEST(JsonUtilsTest, testParseJsonValidJson) {
    // Correct JSON
    std::string valid_json = "{ \"some_property\": \"some_value\"}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_FALSE(parsed_doc.HasParseError());
}

TEST(JsonUtilsTest, testParseJsonValidJsonFile) {
    // Correct JSON
    std::string file_path = "/home/carma-streets/streets_utils/json_utils/test/test_files/valid.json";
    auto parsed_doc = parse_json_file(file_path);
    EXPECT_FALSE(parsed_doc.HasParseError());

}

TEST(JsonUtilsTest, testParseJsonInvalidJsonFilePath) {
    // Correct JSON
    std::string file_path = "/home/invalid/filepath.json";
    EXPECT_THROW(parse_json_file(file_path), std::invalid_argument);
}

TEST(JsonUtilsTest, testParseJsonInvalidJsonFile) {
    // Correct JSON
    std::string file_path = "/home/carma-streets/streets_utils/json_utils/test/test_files/invalid.json";
    EXPECT_THROW(parse_json_file(file_path), json_document_parse_error);
}

//---------------------test parse_uint_member---------------------
TEST(JsonUtilsTest, testGetJsonUintRequiredPropertyPresent){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_EQ( 12345, parse_uint_member("some_property", parsed_doc, true));
}

TEST(JsonUtilsTest, testGetJsonUintRequiredPropertyNotPresent){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_uint_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonUintRequiredPropertyWrongType){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": -12345}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_uint_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonUintOptionalPropertyMissing) {
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    auto property = parse_uint_member("some_property", parsed_doc, false);
    EXPECT_FALSE( property.has_value());
}
//---------------------test get_json_int_property---------------------

TEST(JsonUtilsTest, testGetJsonIntRequiredPropertyPresent){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": -12345}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_EQ( -12345, parse_int_member("some_property", parsed_doc, true));
}

TEST(JsonUtilsTest, testGetJsonIntRequiredPropertyNotPresent){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_int_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonIntRequiredPropertyWrongType){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": true}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_int_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonIntOptionalPropertyMissing) {
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    auto property = parse_int_member("some_property", parsed_doc, false);
    EXPECT_FALSE( property.has_value());
}
//---------------------test parse_bool_property---------------------

TEST(JsonUtilsTest, testGetJsonBoolRequiredPropertyPresent){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": true}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_EQ( true, parse_bool_member("some_property", parsed_doc, true));
}

TEST(JsonUtilsTest, testGetJsonBoolRequiredPropertyNotPresent){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": true}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_bool_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonBoolRequiredPropertyWrongType){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": 1234}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_bool_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonBoolOptionalPropertyMissing) {
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    auto property = parse_bool_member("some_property", parsed_doc, false);
    EXPECT_FALSE( property.has_value());
}

//---------------------test get_json_double_property---------------------

TEST(JsonUtilsTest, testGetJsonDoubleRequiredPropertyPresent){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": 12.3}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_NEAR( 12.3, parse_double_member("some_property", parsed_doc, true).value(), 0.01);
}

TEST(JsonUtilsTest, testGetJsonDoubleRequiredPropertyNotPresent){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12.3}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_double_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonDoubleRequiredPropertyWrongType){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": 1234}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_double_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonDoubleOptionalPropertyMissing) {
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    auto property = parse_double_member("some_property", parsed_doc, false);
    EXPECT_FALSE( property.has_value());
}
//---------------------test parse_string_property---------------------

TEST(JsonUtilsTest, testGetJsonStringRequiredPropertyPresent){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": \"some_property\" }";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_EQ("some_property", parse_string_member("some_property", parsed_doc, true));
}

TEST(JsonUtilsTest, testGetJsonStringRequiredPropertyNotPresent){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12.3}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_string_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonStringRequiredPropertyWrongType){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": 1234}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_string_member("some_property", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonStringOptionalPropertyMissing) {
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    auto property = parse_string_member("some_property", parsed_doc, false);
    EXPECT_FALSE( property.has_value());
}

//---------------------test parse_object_property---------------------

TEST(JsonUtilsTest, testGetJsonObjectRequiredPropertyPresent){
    // Test with required property present
    std::string valid_json = "{ "
                "\"some_object\": {"
                    "\"object_name\" : \"object\","
                    "\"object_value\" : 123"
                "}" 
            "}";
    auto parsed_doc = parse_json(valid_json);
    auto object = parse_object_member("some_object", parsed_doc, true);
    EXPECT_EQ("object", parse_string_member("object_name", object.value(), true));
    EXPECT_EQ( 123, parse_int_member("object_value", object.value(), true) );

}

TEST(JsonUtilsTest, testGetJsonObjectRequiredPropertyNotPresent){
    // Test with required property no present
    std::string valid_json = "{ "
                "\"some_other_object\": {"
                    "\"object_name\" : \"object\","
                    "\"object_value\" : 123"
                "}" 
            "}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_object_member("some_object", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonObjectRequiredPropertyWrongType){
    // Test with required property present with wrong type
     std::string valid_json = "{ "
                "\"some_object\": [{"
                    "\"object_name\" : \"object\","
                    "\"object_value\" : 123"
                "}]" 
            "}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_object_member("some_object", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonObjectOptionalPropertyMissing) {
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    auto property = parse_object_member("some_property", parsed_doc, false);
    EXPECT_FALSE( property.has_value());
}
//---------------------test parse_array_property---------------------

TEST(JsonUtilsTest, testGetJsonArrayRequiredPropertyPresent){
    // Test with required property present
    std::string valid_json = "{ "
                "\"some_array\": "
                    "[456, 2452, -1232, 2345]" 
                "}";
    auto parsed_doc = parse_json(valid_json);
    auto array = parse_array_member("some_array", parsed_doc, true);

    EXPECT_EQ(4, array.value().Size());
    EXPECT_EQ(456, array.value()[0].GetInt());
    EXPECT_EQ(2452, array.value()[1].GetInt());
    EXPECT_EQ(-1232, array.value()[2].GetInt());
    EXPECT_EQ(2345, array.value()[3].GetInt());
}

TEST(JsonUtilsTest, testGetJsonArrayRequiredPropertyNotPresent){
    // Test with required property no present
    std::string valid_json = "{ "
                "\"some_other_array\": "
                    "[456, 2452, -1232, 2345]" 
                "}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_array_member("some_array", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonArrayRequiredPropertyWrongType){
    // Test with required property present with wrong type
     std::string valid_json = "{ "
                "\"some_array\": [{"
                    "\"object_name\" : \"object\","
                    "\"object_value\" : 123"
                "}]" 
            "}";
    auto parsed_doc = parse_json(valid_json);
    EXPECT_THROW( parse_array_member("some_object", parsed_doc, true), json_parse_exception);
}

TEST(JsonUtilsTest, testGetJsonArrayOptionalPropertyMissing) {
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = parse_json(valid_json);
    auto property = parse_array_member("some_property", parsed_doc, false);
    EXPECT_FALSE( property.has_value());
}


