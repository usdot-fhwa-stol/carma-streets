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
    // Minute of the year
    intersection_state state;
    // Enabled lane list
    state.enabled_lanes.push_back(1);
    state.enabled_lanes.push_back(3);
    state.enabled_lanes.push_back(5);

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
    state.revision = 123;
    // Add minute of the year
    state.moy = 34232;
    // Add second of the minute
    state.time_stamp =  13;
    // Add name
    state.name = "West Intersection";
    // Add status
    state.status = 6;
    // Add movement states
    movement_state move_state;
    move_state.maneuver_assist_list.push_back(manuever);
    move_state.movement_name = "Right Turn";
    move_state.signal_group = 4;

    // Add movement event
    movement_event event;
    event.event_state = movement_phase_state::stop_and_remain;
    event.timing.start_time = 0;
    event.timing.min_end_time =10000;
    // Add advisory speed
    advisory_speed speed;
    speed.confidence = speed_confidence::pre100ms;
    speed.distance = 5;
    speed.speed = 4;
    speed.veh_class= 5;
    event.speeds.push_back(speed);
    // Add Movement Event to event list
    move_state.state_time_speed.push_back(event);
    // Add MovementState to states list in IntersectionState
    state.states.push_back(move_state);
    // Add IntersectionState to SPat
    spat_message.set_intersection(state);
    spat_message.set_timestamp_local();


    std::string msg_to_send = spat_message.toJson();
    ASSERT_FALSE(msg_to_send.empty());
    // Read JSON string into new spat object
    spat json_spat;
    json_spat.fromJson(msg_to_send);
    // Assert equal to original object
    ASSERT_EQ(json_spat, spat_message);

}
/**
 * @brief Test fromJson method with optional fields
 * 
 */
TEST (spat_to_json, sample_string_read ) {
    spat spat_message;
    std::string json = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"revision\":123,\"status\":7,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[1,3,5],\"states\":[{\"movement_name\":\"Right Turn\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":0,\"min_end_time\":0,\"max_end_time\":0,\"likely_time\":0,\"confidence\":0},\"speeds\":[{\"type\":0,\"speed_limit\":4,\"speed_confidence\":1,\"distance\":5,\"class\":5}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    spat_message.fromJson(json);
    ASSERT_EQ(spat_message.get_name(), "West Intersection");
    ASSERT_EQ(spat_message.get_timestamp(), 0);
    intersection_state intersection = spat_message.get_intersection();
    ASSERT_EQ(intersection.id, 1909);
    ASSERT_EQ(intersection.status, 7);
    ASSERT_EQ(intersection.revision,123);
    ASSERT_EQ(intersection.maneuver_assist_list.size(), 1);
    ASSERT_EQ( intersection.moy, 34232);
    ASSERT_EQ(intersection.time_stamp,130);
    ASSERT_EQ(intersection.states.size(),1);
}

/**
 * @brief Test fromJson method with missing optional string fields
 * 
 */
TEST (spat_to_json, sample_string_read_without_name ) {
    spat spat_message;
    std::string json = "{\"timestamp\":0,\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"revision\":123,\"status\":10,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[1,3,5],\"states\":[{\"movement_name\":\"Right Turn\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":0,\"min_end_time\":0,\"max_end_time\":0,\"likely_time\":0,\"confidence\":0},\"speeds\":[{\"type\":0,\"speed_limit\":4,\"speed_confidence\":1,\"distance\":5,\"class\":5}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    spat_message.fromJson(json);
    ASSERT_TRUE(spat_message.get_name().empty());
    intersection_state intersection = spat_message.get_intersection();
    ASSERT_EQ(intersection.id, 1909);
    ASSERT_EQ(intersection.status, 10);
    ASSERT_EQ(intersection.revision,123);
    ASSERT_EQ(intersection.maneuver_assist_list.size(), 1);
    ASSERT_EQ( intersection.moy, 34232);
    ASSERT_EQ(intersection.time_stamp,130);
    ASSERT_EQ(intersection.states.size(),1);
}

/**
 * @brief Test fromJson method with missing required integer fields
 * 
 */
TEST (spat_to_json, sample_string_read_without_revision ) {
    spat spat_message;
    std::string json = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":10,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[1,3,5],\"states\":[{\"movement_name\":\"Right Turn\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":0,\"min_end_time\":0,\"max_end_time\":0,\"likely_time\":0,\"confidence\":0},\"speeds\":[{\"type\":0,\"speed_limit\":4,\"speed_confidence\":1,\"distance\":5,\"class\":5}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    ASSERT_THROW(spat_message.fromJson(json), signal_phase_and_timing_exception);;
}

/**
 * @brief Test fromJson method with missing optional integer fields
 * 
 */
TEST (spat_to_json, sample_string_read_optional_timemarks ) {
    spat spat_message;
    std::string json = "{\"timestamp\":0,\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"revision\":123,\"status\":10,\"moy\":34232,\"time_stamp\":130,\"states\":[{\"movement_name\":\"Right Turn\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":0,\"min_end_time\":1000},\"speeds\":[{\"type\":0,\"speed_limit\":4,\"speed_confidence\":1,\"distance\":5,\"class\":5}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    spat_message.fromJson(json);
    ASSERT_TRUE(spat_message.get_name().empty());    
    intersection_state intersection = spat_message.get_intersection();
    ASSERT_EQ(intersection.id, 1909);
    ASSERT_EQ(intersection.status, 10);
    ASSERT_EQ(intersection.revision,123);
    ASSERT_EQ(intersection.maneuver_assist_list.size(), 1);
    ASSERT_EQ( intersection.moy, 34232);
    ASSERT_EQ(intersection.time_stamp,130);
    ASSERT_EQ(intersection.states.size(),1);
    movement_state move_state =  intersection.states.front();
    ASSERT_EQ( move_state.state_time_speed.size(),1 );
    ASSERT_EQ( move_state.state_time_speed.front().timing.max_end_time, 36001);
    ASSERT_EQ( move_state.state_time_speed.front().timing.start_time, 0);
    ASSERT_EQ( move_state.state_time_speed.front().timing.min_end_time, 1000);


}

/**
 * @brief Test to Json conversion serialization
 */
TEST(spat_to_json, missing_state_list)  {
    spat spat_message;
    ASSERT_THROW(spat_message.set_timestamp_local(), signal_phase_and_timing_exception);
    
}


/**
 * @brief Test to Json conversion serialization
 */
TEST(spat_to_json, missing_intersection_id)  {
    spat spat_message;
    intersection_state state;
    spat_message.set_intersection(state);
    spat_message.set_timestamp_local();
    

    // Write JSON Value to string
    rapidjson::Document doc;
    ASSERT_THROW(spat_message.toJson(), signal_phase_and_timing_exception);
    
}

/**
 * @brief Test to Json conversion serialization
 */
TEST(spat_to_json, missing_message_count)  {
    spat spat_message;
    intersection_state state;
    state.id=1902;
    spat_message.set_intersection(state);
    spat_message.set_timestamp_local();

    // Write JSON Value to string
    rapidjson::Document doc;
    ASSERT_THROW(spat_message.toJson(), signal_phase_and_timing_exception);
    
}

/**
 * @brief Test to Json conversion serialization
 */
TEST(spat_to_json, missing_intersection_status)  {
    spat spat_message;
    intersection_state state;
    state.id=1902;
    state.revision = 202;
    spat_message.set_intersection(state);
    spat_message.set_timestamp_local();

    // Write JSON Value to string
    rapidjson::Document doc;
    ASSERT_THROW(spat_message.toJson(), signal_phase_and_timing_exception);
    
}

/**
 * @brief Test to Json conversion serialization
 */
TEST(spat_to_json, missing_movement_event_list)  {
    spat spat_message;
    intersection_state state;
    // Add intersection id 
    state.id=1902;
    // Add message count
    state.revision = 202;
    // Add status
    state.status = 67;
    // Add state
    spat_message.set_intersection(state);
    spat_message.set_timestamp_local();

    // Write JSON Value to string
    rapidjson::Document doc;
    ASSERT_THROW(spat_message.toJson(), signal_phase_and_timing_exception);
}

/**
 * @brief Test to Json conversion serialization
 */
TEST(spat_to_json, missing_movement_state_signal_group_id)  {
    spat spat_message;
    intersection_state state;
    // Add intersection id 
    state.id=1902;
    // Add message count
    state.revision = 202;
    // Add status
    state.status = 12;
    // Add Movement state
    movement_state movement_state;
    state.states.push_back(movement_state);
    // Add state
    spat_message.set_intersection(state);
    spat_message.set_timestamp_local();

    // Write JSON Value to string
    rapidjson::Document doc;
    ASSERT_THROW(spat_message.toJson(), signal_phase_and_timing_exception);
}



