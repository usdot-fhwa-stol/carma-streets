#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "spat.h"
#include "signal_phase_and_timing_exception.h"

using namespace signal_phase_and_timing;


/**
 * @brief Test to Json conversion serialization and deserialization
 */
TEST(spat_to_json, to_from_json_test) {
    spat spat_message;
    spat_message.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
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
    event.timing.start_time = spat_message.timestamp;
    event.timing.min_end_time = spat_message.timestamp + 10000;
    // Add advisory speed
    advisory_speed speed;
    speed.confidence = speed_confidence::pre100ms;
    speed.distance = 5;
    speed.speed_limit = 4;
    speed.veh_class= 5;
    event.speeds.push_back(speed);
    // Add Movement Event to event list
    move_state.movement_event_list.push_back(event);
    // Add MovementState to states list in IntersectionState
    state.movement_states.push_back(move_state);
    // Add IntersectionState to SPat
    spat_message.intersection_state_list.push_front(state);
    // Write JSON Value to string
    rapidjson::Document doc;
    rapidjson::Value val = spat_message.toJson(doc.GetAllocator());
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    val.Accept(writer);
    std::string msg_to_send = buffer.GetString();
    SPDLOG_INFO("SPaT Json {0}", msg_to_send);

    ASSERT_FALSE(msg_to_send.empty());
    // Read JSON string into new spat object
    spat json_spat;
    doc.Parse(msg_to_send);
    ASSERT_FALSE(doc.HasParseError());
    json_spat.fromJson(doc);
    // Assert equal to original object
    ASSERT_EQ(json_spat, spat_message);

}

/**
 * @brief Test to Json conversion serialization and deserialization
 */
TEST(spat_to_json, missing_intersection_id_property)  {
    spat spat_message;
    spat_message.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    intersection_state state;
    
    // Enabled lane list
    state.enabled_lane_list.push_back(1);
    state.enabled_lane_list.push_back(3);
    state.enabled_lane_list.push_back(5);

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
    event.timing.start_time = spat_message.timestamp;
    event.timing.min_end_time = spat_message.timestamp + 10000;
    event.event_state = movement_phase_state::stop_and_remain;
    // Add advisory speed
    advisory_speed speed;
    speed.confidence = speed_confidence::pre100ms;
    speed.distance = 5;
    speed.speed_limit = 4;
    speed.veh_class= 5;
    event.speeds.push_back(speed);
    move_state.movement_event_list.push_back(event);
    // Add MovementState to states list in IntersectionState
    state.movement_states.push_back(move_state);
    // Add IntersectionState to SPat
    spat_message.intersection_state_list.push_front(state);
    // Write JSON Value to string
    rapidjson::Document doc;
 
    
    ASSERT_THROW(spat_message.toJson(doc.GetAllocator()), signal_phase_and_timing_exception);
    
     


}

