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

    signal_phase_and_timing::signal_group_configuration state;
    state.fromJson(signal_group_config_json);
    ASSERT_EQ(10, state.yellow_change_duration);

    rapidjson::Value config_json(rapidjson::kObjectType);
    rapidjson::Value tsc_configurations(rapidjson::kArrayType);
    tsc_configurations.PushBack(signal_group_config_json, allocator);
    config_json.AddMember("tsc_config_list", tsc_configurations, allocator);

    signal_phase_and_timing::tsc_configuration_state config_list;
    config_list.fromJson(config_json);
    ASSERT_EQ(10, config_list.tsc_config_list.front().yellow_change_duration);

}

TEST(test_tsc_configuration_state, toJson)
{
    signal_phase_and_timing::signal_group_configuration state;
    rapidjson::Document doc;
    auto allocator = doc.GetAllocator();
    state.signal_group_id = 1;
    state.yellow_change_duration = 10;
    state.red_clearance = 5;
    state.concurrent_signal_groups.push_back(2);
    state.concurrent_signal_groups.push_back(3);

    auto json = state.toJson(allocator);
    ASSERT_EQ(10, json["yellow_change_duration"].GetUint());

    signal_phase_and_timing::tsc_configuration_state config;
    config.tsc_config_list.push_back(state);
    auto config_json = config.toJson(allocator);
    int count = 0;
    for (const auto& itr : config_json["tsc_config_list"].GetArray()){
        ASSERT_EQ(itr["yellow_change_duration"].GetUint(), config.tsc_config_list[count].yellow_change_duration);
        count++;
    }
}
