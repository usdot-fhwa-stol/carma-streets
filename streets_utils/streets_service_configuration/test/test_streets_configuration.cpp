#define RAPIDJSON_HAS_STDSTRING 1
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <fstream>


#include "streets_singleton.h"
#include "streets_configuration.h"

using namespace streets_service;

namespace streets_service{


   class test_streets_configuration : public testing::Test {
      protected:
         void SetUp() {
            SPDLOG_INFO("Setup");
         }
         void TearDown() {
               SPDLOG_INFO("Tear Down");
         }
      public:
      /**
       * @brief Helper methon to create manifest.json file.
       * @param filepath where to create manifest.json file.
       */ 
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
      /**
         TODO: Creating/deleting files like this during unit test execution can be an issue for parallelizing test execution.
         not critical but it can be nice to have if you've got a significant mass of unit tests. For stuff like this I find it better
         to load files as a 2-step process: Read from disk into buffer
         Parse buffer into config object. Then during unit tests you can pre-construct some buffer objects 
         (just hardcode the strings or whatever) and verify that the parsing works as well as the things that 
         use the config. The only thing that doesn't get validated that way is the act of reading from disc and
         normally I don't see the value in validating environment stuff like syscalls (or message framework code) 
         unless you're doing something particularly off the wall and you want to validate that it works.*/
      };

      /**
       * @brief Update string param3 inside manifest.json.
       * @param filepath to manifest.json
       * @param new_value to update param3
       */ 
      void update_configuration(const std::string &filepath, const std::string &new_value){
         std::ifstream out_file(filepath);
         if (!out_file.is_open()) {
            FAIL();
         }
         // Add file contents to stream and parse stream into Document
         rapidjson::IStreamWrapper isw(out_file);
         rapidjson::Document doc;
         doc.ParseStream(isw);
         out_file.close();
         if ( doc.FindMember("configurations")->value.IsArray()){
            if  (doc.FindMember("configurations")->value.GetArray()[2].IsObject()){
               if (doc.FindMember("configurations")->value.GetArray()[2].GetObject().FindMember("value")->value.IsString()) {
                  doc.FindMember("configurations")->value.GetArray()[2].GetObject().FindMember("value")->value.SetString(new_value,doc.GetAllocator());
                  SPDLOG_INFO("Updated {0} to {1}!",  doc.FindMember("configurations")->value.GetArray()[2].GetObject().FindMember("name")->value.GetString(), new_value );
               }
            }
         }

         std::ofstream in_file(filepath);
         rapidjson::OStreamWrapper osw(in_file);
         rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
         doc.Accept(writer);
         in_file.close();
      }
      /**
       * Remove all .json configuration files 
       */
      void clear_configuration_files(){
         std::remove( "../manifest.json");
      }

   };



   /**
    * @brief Tested singleton initialization without manifest.json configuration file. 
    */ 
   TEST_F(test_streets_configuration, missing_configuration_file)
   {
      clear_configuration_files();
      EXPECT_THROW(streets_configuration::create("../manifest.json"), streets_configuration_exception);
      clear_configuration_files();

   };
   /**
    * @brief Tested get_config methods with created manifest.json file including updates
    */ 
   TEST_F(test_streets_configuration, get_config) {
      create_test_configuration("../manifest.json");
      streets_configuration::create("../manifest.json");
      


      streets_configuration::initialize_logger();
      SPDLOG_INFO("Calling methods");

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
      // sleep for a second to allow last modified timestamp to change
      sleep(1);
      // update values
      update_configuration( "../manifest.json", "UPDATED");   

      ASSERT_EQ(streets_configuration::get_string_config("param3"), "UPDATED");
      // Clean up created configuration files
      clear_configuration_files();
   };
}
