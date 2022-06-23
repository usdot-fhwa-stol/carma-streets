#include <gtest/gtest.h>
#define SPDLOG_ACTIVE_LEVEL SPDLOG_DEBUG

#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "spat.h"
using namespace signal_phase_and_timing;
namespace {
    /**
     * @brief Test class to test intersection_schedule toCSV and toJSON methods.
     * 
     */
    class spat_to_json : public ::testing::Test {
    private:

       

    protected:
        
     

        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            spdlog::set_level(spdlog::level::debug);
        }
            
        /**
         * @brief Test TearDown method run after each test.
         * 
         */
        void TearDown() override{
            
        }
    };
};

/**
 * @brief Test to Json conversion
 * 
 */
TEST_F(spat_to_json, json_test) {
    spat spat_message;
    intersection_state state;
    // Enabled lane list
    state.enabled_lane_list.push_back(1);
    state.enabled_lane_list.push_back(3);
    state.enabled_lane_list.push_back(5);

    // Set intersection id
    state.id = 1909;
    // Create manuever
    connection_maneuver_assist manuever;
    manuever.available_storage_length = 8;
    manuever.connection_id = 7;
    manuever.ped_bicycle_detect = false;
    manuever.queue_length = 4;
    manuever.wait_on_stop = true;
    // Add manuever
    state.maneuver_assist_list.push_back(manuever);
    // Add message count
    state.message_count = 12332;
    // Add minute of the year
    state.minute_of_the_year = 34232;
    // Add second of the minute
    state.second =  13;
    // Add name
    state.name = "West Intersection";
    // Add status
    state.status = "3e21f0923e21f092";
    // Add movement states
    movement_state move_state;
    move_state.maneuver_assist_list.push_back(manuever);
    move_state.movement_name = "Right Turn";
    move_state.signal_group_id = 4;
    // Add movement event
    movement_event event;
    event.event_state = movement_phase_state::stop_and_remain;
    // Add advisory speed
    advisory_speed speed;
    speed.confidence = speed_confidence::pre100ms;
    speed.distance = 5;
    speed.speed_limit = 4;
    speed.veh_class= 5;
    event.speeds.push_back(speed);
    move_state.movement_event_list.push_back(event);
    
    state.movement_states.push_back(move_state);

    spat_message.intersection_state_list.push_front(state);
    rapidjson::Document doc;
    rapidjson::Value val = spat_message.toJson(doc.GetAllocator());
    SPDLOG_INFO("SPat JSON value created.");
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    val.Accept(writer);

    std::string msg_to_send = buffer.GetString();
    SPDLOG_INFO("SPaT Json {0}", msg_to_send);
    ASSERT_FALSE(msg_to_send.empty());

    spat json_spat;

    doc.Parse(msg_to_send);

    ASSERT_FALSE(doc.HasParseError());

    json_spat.fromJson(doc);

    ASSERT_TRUE(json_spat == spat_message);
    
}