#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "tsc_configuration_state.h"

TEST(test_signal_group_configuration, fromJson)
{
    rapidjson::Value signal_group_config_json(rapidjson::kObjectType);
    rapidjson::Document doc;
    auto allocator = doc.GetAllocator();
    signal_group_config_json.AddMember("signal_group_id", 1, allocator);
    signal_group_config_json.AddMember("yellow_change_duration", 10, allocator);
    signal_group_config_json.AddMember("red_clearance", 5, allocator);
    rapidjson::Value signal_groups(rapidjson::kArrayType);
    signal_group_config_json.AddMember("concurrent_signal_groups", signal_groups, allocator);

    streets_tsc_configuration::signal_group_configuration state;
    state.fromJson(signal_group_config_json);
    ASSERT_EQ(10, state.yellow_change_duration);

    rapidjson::Value config_json(rapidjson::kObjectType);
    rapidjson::Value tsc_configurations(rapidjson::kArrayType);
    tsc_configurations.PushBack(signal_group_config_json, allocator);
    config_json.AddMember("tsc_config_list", tsc_configurations, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    config_json.Accept(writer);


    streets_tsc_configuration::tsc_configuration_state config_list;
    config_list.fromJson(buffer.GetString());
    ASSERT_EQ(10, config_list.tsc_config_list.front().yellow_change_duration);
    ASSERT_TRUE(config_list.tsc_config_list.front().concurrent_signal_groups.empty());

}

TEST(test_tsc_configuration_state, toJson)
{
    streets_tsc_configuration::signal_group_configuration state;
    rapidjson::Document doc;
    auto allocator = doc.GetAllocator();
    state.signal_group_id = 1;
    state.yellow_change_duration = 10;
    state.red_clearance = 5;
    state.concurrent_signal_groups.push_back(2);
    state.concurrent_signal_groups.push_back(3);

    auto json = state.toJson(allocator);
    ASSERT_EQ(10, json["yellow_change_duration"].GetUint());

    streets_tsc_configuration::tsc_configuration_state config;
    config.tsc_config_list.push_back(state);
    auto config_json = config.toJson();
    ASSERT_FALSE(config_json.empty());
    
}
