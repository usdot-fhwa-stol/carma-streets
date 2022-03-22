#include <gtest/gtest.h>
#include <streets_singleton.h>
#include <streets_configuration.h>
#include <spdlog/spdlog.h>
#include <rapidjson/ostreamwrapper.h>
#include <fstream>

using namespace streets_service;

void create_test_configuration(const std::string &filepath){
   rapidjson::Document doc;
   doc.SetObject();
   auto allocator = doc.GetAllocator() ;
   // Service Level Configuration
   doc.AddMember("service_name", "test_service", allocator);
   doc.AddMember("loglevel", "trace", allocator);
   // Add Configurations
   rapidjson::Value conf_array(rapidjson::kArrayType);
   // Configuration 1
   rapidjson::Value conf1(rapidjson::kObjectType);
   conf1.AddMember("name", "param1", allocator);
   conf1.AddMember("value", 123, allocator);
   conf1.AddMember("description", "Test Parameter 1", allocator);
   conf1.AddMember("type", "INTEGER", allocator);
   // Add to array
   conf_array.PushBack(conf1, allocator);
   // Configuration 2
   rapidjson::Value conf2(rapidjson::kObjectType);
   conf2.AddMember("name", "param2", allocator);
   conf2.AddMember("value", true, allocator);
   conf2.AddMember("description", "Test Parameter 2", allocator);
   conf2.AddMember("type", "BOOL", allocator);
   // Add to array
   conf_array.PushBack(conf2, allocator);
   // Configuration 3
   rapidjson::Value conf3(rapidjson::kObjectType);
   conf3.AddMember("name", "param3", allocator);
   conf3.AddMember("value", "TESTING", allocator);
   conf3.AddMember("description", "Test Parameter 3", allocator);
   conf3.AddMember("type", "STRING", allocator);
   // Add to array
   conf_array.PushBack(conf3, allocator);
   // Configuration 2
   rapidjson::Value conf4(rapidjson::kObjectType);
   conf4.AddMember("name", "param4", allocator);
   conf4.AddMember("value", 24.2, allocator);
   conf4.AddMember("description", "Test Parameter 4", allocator);
   conf4.AddMember("type", "DOUBLE", allocator);
   // Add to array
   conf_array.PushBack(conf4, allocator);
   // Add to main json doc
   doc.AddMember("configurations", conf_array, allocator);

   std::ofstream file(filepath);
   rapidjson::OStreamWrapper osw(file);
   rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
   doc.Accept(writer);
   file.close();
};

void clear_configuration_files(){
   std::remove( "../manifest.json");
}



TEST(test_streets_configuration, missing_configuration_file)
{
   EXPECT_THROW(streets_configuration::get_boolean_config("test"), streets_configuration_exception);
};

TEST(test_streets_configuration, get_config) {
   create_test_configuration("../manifest.json");
   // Test Correct Parameters
   ASSERT_TRUE(streets_configuration::get_boolean_config("param2"));
   ASSERT_EQ(streets_configuration::get_int_config("param1"), 123);
   ASSERT_EQ(streets_configuration::get_string_config("param3"), "TESTING");
   ASSERT_DOUBLE_EQ(streets_configuration::get_double_config("param4"),24.2);

   // Test Incorrect types
   EXPECT_THROW(streets_configuration::get_boolean_config("param1"),streets_configuration_exception);
   EXPECT_THROW(streets_configuration::get_string_config("param2"),streets_configuration_exception);
   EXPECT_THROW(streets_configuration::get_double_config("param3"),streets_configuration_exception);
   EXPECT_THROW(streets_configuration::get_int_config("param4"),streets_configuration_exception);
   clear_configuration_files();
};
