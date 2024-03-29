#include "vehicle.h"
#include "vehicle_list.h"
#include "status_intent_processing_exception.h"
#include "status_intent_processor.h"
#include "all_stop_status_intent_processor.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <fstream>

using namespace streets_vehicles;

namespace {

    class vehicle_list_test : public ::testing::Test {
    private:

       

    protected:
        std::unique_ptr<vehicle_list> veh_list;
        /**
         * @brief Helper method to load vehicle status and intent updates into a vector 
         * of strings using a filepath as a parameter.
         * 
         * @param filepath to json file of vehicle status and intent updates for testing.
         * @return std::vector<std::string> 
         */
        std::vector<std::string> load_vehicle_update(const std::string &filepath){
            std::ifstream file(filepath);
            if (!file.is_open()) {
                throw streets_vehicles::status_intent_processing_exception("Unable to open status and intent message update file " + filepath + " !"); 
            }
            // Add file contents to stream and parse stream into Document
            rapidjson::IStreamWrapper isw(file);
            rapidjson::Document doc;
            doc.ParseStream(isw);
            if (doc.HasParseError()){
                SPDLOG_ERROR("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
                throw streets_vehicles::status_intent_processing_exception("Document parse error!");
            }
            file.close();
            std::vector<std::string> updates;
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            if ( doc.FindMember("vehicle_updates")->value.IsArray() ) {
                for ( auto& update: doc.FindMember("vehicle_updates")->value.GetArray() ) {
                    if (update.IsObject()) {
                        buffer.Clear();
                        writer.Reset(buffer);           
                        update.Accept(writer);
                        std::string up = buffer.GetString();
                        updates.push_back(up);
                    }
                }
                return updates;
            }
                
        }
        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            veh_list = std::unique_ptr<vehicle_list>(new vehicle_list());
            // Setup All_stop_status_intent_processor
            veh_list->set_processor(std::make_shared<all_stop_status_intent_processor>());
            auto processor = std::dynamic_pointer_cast<all_stop_status_intent_processor>(veh_list->get_processor());
            processor->set_stopping_distance(1.0);
            processor->set_stopping_speed(0.1);
            SPDLOG_INFO("Test SetUp Complete");
        }
        /**
         * @brief Test TearDown method run after each test.
         * 
         */
        void TearDown() override{
            
        }
        
    };
};




TEST_F(vehicle_list_test, parse_valid_json) {
    // Test initialization
    ASSERT_EQ(veh_list->get_vehicles().size(), 0);
    // Set timeout to 10 year in milliseconds.
    veh_list->get_processor()->set_timeout(3.154e11);
    // Print timeout in days.
    SPDLOG_INFO( "Set timeout to {0} days !",veh_list->get_processor()->get_timeout()/(1000*60*60*24));
    
    // Load Vehicle Update
    std::vector<std::string> updates = load_vehicle_update("../test/test_data/updates.json");
    int i = 0;
    for ( auto& update: updates ) {
        SPDLOG_INFO("Processing Update {0} ", update);
        veh_list->process_update(update);
       
        if ( i == 0) {
            ASSERT_EQ( veh_list->get_vehicles().size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::EV).size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::EV).begin()->_id, "DOT-507");
            ASSERT_EQ( veh_list->get_vehicles_by_lane(7).begin()->_id, "DOT-507");
        }
        else if ( i == 1) {
            ASSERT_EQ( veh_list->get_vehicles().size(), 2);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::EV).size(), 2);
            ASSERT_EQ( veh_list->get_vehicles_by_lane(5).size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_lane(5).begin()->_id, "DOT-508");
        }
        else if (i == 4) {
            ASSERT_EQ( veh_list->get_vehicles().size(), 2);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::RDV).size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::RDV).begin()->_id, "DOT-507");
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::EV).size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::EV).begin()->_id, "DOT-508");
        }
        else if (i == 5) {
            ASSERT_EQ( veh_list->get_vehicles().size(), 2);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::RDV).size(), 2);
        }
        else if (i == 6) {
            SPDLOG_INFO("Vehicle update departure position {0}", veh_list->get_vehicles().find("DOT-45244")->second._departure_position);
        }
        else {
            ASSERT_EQ( veh_list->get_vehicles().size(), 2);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::EV).size(), 2);
            ASSERT_EQ( veh_list->get_vehicles_by_lane(5).size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_lane(5).begin()->_id, "DOT-508");
            ASSERT_EQ( veh_list->get_vehicles_by_lane(7).size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_lane(7).begin()->_id, "DOT-507");
        }
        SPDLOG_INFO("Update Processed");
        i++;
    }
    SPDLOG_INFO("Processed all updates!");

}

TEST_F(vehicle_list_test, parse_invalid_json) {
    // Test initialization
    auto vehicles = veh_list->get_vehicles();
    ASSERT_EQ(vehicles.size(), 0);
    // Set timeout to 10 year in milliseconds.
    veh_list->get_processor()->set_timeout(3.156e11);
    // Print timeout in days.
    SPDLOG_INFO( "Set timeout to {0} days !", veh_list->get_processor()->get_timeout()/(1000*60*60*24));
    
    // Load Vehicle Update
    std::vector<std::string> updates = load_vehicle_update("../test/test_data/updates_missing_fields.json");
    int i = 0;
    for ( auto& update: updates ) {
        SPDLOG_INFO("Processing  Update {0} : {1} ", i, update);
        veh_list->process_update(update);
        if ( i == updates.size()-1) {
            ASSERT_EQ( veh_list->get_vehicles().size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::EV).size(), 1);
            ASSERT_EQ( veh_list->get_vehicles_by_state(vehicle_state::EV).begin()->_id, "DOT-507");
            ASSERT_EQ( veh_list->get_vehicles_by_lane(7).begin()->_id, "DOT-507");
        }
        else {
            ASSERT_EQ( veh_list->get_vehicles().size(), 0);
        }
        SPDLOG_INFO("Update Processed");
        i++;
    }
    SPDLOG_INFO("Processed all updates!");

}

TEST_F(vehicle_list_test, parse_timeout_json) {
    // Test initialization
    auto vehicles =veh_list->get_vehicles();
    ASSERT_EQ(vehicles.size(), 0);
    // Set timeout to 30 seconds in milliseconds.
    veh_list->get_processor()->set_timeout(30000);
    // Print timeout in seconds.
    SPDLOG_INFO( "Set timeout to {0} days !", veh_list->get_processor()->get_timeout()/(1000));
    
    // Load Vehicle Update
    std::vector<std::string> updates = load_vehicle_update("../test/test_data/updates.json");
    int i = 0;
    for ( auto& update: updates ) {
        SPDLOG_INFO("Processing  Update {0} : {1} ", i, update);
        veh_list->process_update(update);
        ASSERT_EQ( veh_list->get_vehicles().size(), 0);
        SPDLOG_INFO("Update Processed");
        i++;
    }
    SPDLOG_INFO("Processed all updates!");

}