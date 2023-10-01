#include <gtest/gtest.h>
#include <json_utility.hpp>

using namespace streets_utils::json_utils;

//---------------------test validate_json---------------------
TEST(json_utility_test, test_validate_invalid_json) {
    // Empty String
    std::string invalid_json = "";
    EXPECT_THROW( validate_json(invalid_json), std::runtime_error);

    // Property missing quotations
    invalid_json = "{ some_propert: \"some_value\"}";
    EXPECT_THROW( validate_json(invalid_json), std::runtime_error);

}

TEST(json_utility_test, test_validate_valid_json) {
    // Correct JSON
    std::string valid_json = "{ \"some_property\": \"some_value\"}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_FALSE(parsed_doc.HasParseError());
}
//---------------------test parse_uint_property---------------------
TEST(json_utility_test, test_get_json_uint_required_property_present){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": 12345}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_EQ( 12345, parse_uint_property("some_property", parsed_doc, true));
}

TEST(json_utility_test, test_get_json_uint_required_property_not_present){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_uint_property("some_property", parsed_doc, true), std::runtime_error);
}

TEST(json_utility_test, test_get_json_uint_required_property_wrong_type){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": -12345}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_uint_property("some_property", parsed_doc, true), std::runtime_error);
}
//---------------------test get_json_int_property---------------------

TEST(json_utility_test, test_get_json_int_required_property_present){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": -12345}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_EQ( -12345, parse_int_property("some_property", parsed_doc, true));
}

TEST(json_utility_test, test_get_json_int_required_property_not_present){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12345}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_int_property("some_property", parsed_doc, true), std::runtime_error);
}

TEST(json_utility_test, test_get_json_int_required_property_wrong_type){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": true}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_int_property("some_property", parsed_doc, true), std::runtime_error);
}

//---------------------test parse_bool_property---------------------

TEST(json_utility_test, test_get_json_bool_required_property_present){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": true}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_EQ( true, parse_bool_property("some_property", parsed_doc, true));
}

TEST(json_utility_test, test_get_json_bool_required_property_not_present){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": true}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_bool_property("some_property", parsed_doc, true), std::runtime_error);
}

TEST(json_utility_test, test_get_json_bool_required_property_wrong_type){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": 1234}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_bool_property("some_property", parsed_doc, true), std::runtime_error);
}

//---------------------test get_json_double_property---------------------

TEST(json_utility_test, test_get_json_double_required_property_present){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": 12.3}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_NEAR( 12.3, parse_double_property("some_property", parsed_doc, true).value(), 0.01);
}

TEST(json_utility_test, test_get_json_double_required_property_not_present){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12.3}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_double_property("some_property", parsed_doc, true), std::runtime_error);
}

TEST(json_utility_test, test_get_json_double_required_property_wrong_type){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": 1234}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_double_property("some_property", parsed_doc, true), std::runtime_error);
}

//---------------------test parse_string_property---------------------

TEST(json_utility_test, test_get_json_string_required_property_present){
    // Test with required property present
    std::string valid_json = "{ \"some_property\": \"some_property\" }";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_EQ("some_property", parse_string_property("some_property", parsed_doc, true));
}

TEST(json_utility_test, test_get_json_string_required_property_not_present){
    // Test with required property no present
    std::string valid_json = "{ \"some_property_other\": 12.3}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_string_property("some_property", parsed_doc, true), std::runtime_error);
}

TEST(json_utility_test, test_get_json_string_required_property_wrong_type){
    // Test with required property present with wrong type
    std::string valid_json = "{ \"some_property\": 1234}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_string_property("some_property", parsed_doc, true), std::runtime_error);
}


//---------------------test parse_object_property---------------------

TEST(json_utility_test, test_get_json_object_required_property_present){
    // Test with required property present
    std::string valid_json = "{ "
                "\"some_object\": {"
                    "\"object_name\" : \"object\","
                    "\"object_value\" : 123"
                "}" 
            "}";
    auto parsed_doc = validate_json(valid_json);
    auto object = parse_object_property("some_object", parsed_doc, true);
    EXPECT_EQ("object", parse_string_property("object_name", object.value(), true));
    EXPECT_EQ( 123, parse_int_property("object_value", object.value(), true) );

}

TEST(json_utility_test, test_get_json_object_required_property_not_present){
    // Test with required property no present
    std::string valid_json = "{ "
                "\"some_other_object\": {"
                    "\"object_name\" : \"object\","
                    "\"object_value\" : 123"
                "}" 
            "}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_object_property("some_object", parsed_doc, true), std::runtime_error);
}

TEST(json_utility_test, test_get_json_object_required_property_wrong_type){
    // Test with required property present with wrong type
     std::string valid_json = "{ "
                "\"some_object\": [{"
                    "\"object_name\" : \"object\","
                    "\"object_value\" : 123"
                "}]" 
            "}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_object_property("some_object", parsed_doc, true), std::runtime_error);
}

//---------------------test parse_array_property---------------------

TEST(json_utility_test, test_get_json_array_required_property_present){
    // Test with required property present
    std::string valid_json = "{ "
                "\"some_array\": "
                    "[456, 2452, -1232, 2345]" 
                "}";
    auto parsed_doc = validate_json(valid_json);
    auto array = parse_array_property("some_array", parsed_doc, true);

    EXPECT_EQ(4, array.value().Size());
    EXPECT_EQ(456, array.value()[0].GetInt());
    EXPECT_EQ(2452, array.value()[1].GetInt());
    EXPECT_EQ(-1232, array.value()[2].GetInt());
    EXPECT_EQ(2345, array.value()[3].GetInt());
}

TEST(json_utility_test, test_get_json_array_required_property_not_present){
    // Test with required property no present
    std::string valid_json = "{ "
                "\"some_other_array\": "
                    "[456, 2452, -1232, 2345]" 
                "}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_array_property("some_array", parsed_doc, true), std::runtime_error);
}

TEST(json_utility_test, test_get_json_array_required_property_wrong_type){
    // Test with required property present with wrong type
     std::string valid_json = "{ "
                "\"some_array\": [{"
                    "\"object_name\" : \"object\","
                    "\"object_value\" : 123"
                "}]" 
            "}";
    auto parsed_doc = validate_json(valid_json);
    EXPECT_THROW( parse_array_property("some_object", parsed_doc, true), std::runtime_error);
}

