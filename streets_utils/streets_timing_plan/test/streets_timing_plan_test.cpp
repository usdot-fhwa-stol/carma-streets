
#include "streets_timing_plan.h"
#include "streets_timing_plan_exception.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <fstream>

using namespace streets_timing_plan;

namespace
{

    class streets_timing_plan_test : public ::testing::Test
    {
        /**
         * @brief Test Setup method run before each test.
         */
        void SetUp() override
        {
        }
        /**
         * @brief Test TearDown method run after each test.
         */
        void TearDown() override
        {
        }
    };
};

TEST_F(streets_timing_plan_test, toJson)
{
    streets_timing_plan::streets_timing_plan timing_plan;
    timing_plan.number_of_phase = 8;
    std::vector<int> phase_numbers_v{1, 2, 3, 4, 5, 6, 7, 8};
    std::swap(timing_plan.phase_number_v, phase_numbers_v);
    std::vector<int> pede_walk_v{0, 7, 0, 7, 0, 7, 0, 7};
    std::swap(timing_plan.pedestrian_walk_v, pede_walk_v);
    std::vector<int> pede_clear_v{0, 33, 1, 43, 0, 33, 0, 33};
    std::swap(timing_plan.pedestrian_clear_v, pede_clear_v);
    std::vector<int> min_green_v{4, 15, 4, 15, 4, 15, 4, 15};
    std::swap(timing_plan.min_green_v, min_green_v);
    std::vector<double> passage_v{2.0, 5.0, 2.0, 5.0, 2.0, 5.0, 2.0, 5.0};
    std::swap(timing_plan.passage_v, passage_v);
    std::vector<int> max_green_v{37, 35, 19, 40, 32, 19, 29};
    std::swap(timing_plan.max_green_v, max_green_v);
    std::vector<double> yellow_change_v{3.0, 4.0, 3.0, 3.6, 4.0, 3.0, 3.6};
    std::swap(timing_plan.yellow_change_v, yellow_change_v);
    std::vector<double> red_clear_v{1.0, 2.5, 1.0, 3.40000000000004, 1.0, 2.5, 1.0, 3.4000000000000000000004};
    std::swap(timing_plan.red_clear_v, red_clear_v);
    std::vector<int> phase_ring_v{1, 1, 1, 1, 1, 2, 2, 2, 2};
    std::swap(timing_plan.phase_ring_v, phase_ring_v);

    auto timing_plan_json_value = timing_plan.toJson();
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    timing_plan_json_value.Accept(writer);
    std::string expected_str = "{\"MsgType\":\"ActiveTimingPlan\",\"TimingPlan\":{\"NoOfPhase\":8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7],\"PedClear\":[0,33,1,43,0,33,0,33],\"MinGreen\":[4,15,4,15,4,15,4,15],\"Passage\":[2.0,5.0,2.0,5.0,2.0,5.0,2.0,5.0],\"MaxGreen\":[37,35,19,40,32,19,29],\"YellowChange\":[3.0,4.0,3.0,3.6,4.0,3.0,3.6],\"RedClear\":[1.0,2.5,1.0,3.40000000000004,1.0,2.5,1.0,3.4],\"PhaseRing\":[1,1,1,1,1,2,2,2,2]}}";
    ASSERT_EQ(expected_str, strbuf.GetString());
}

TEST_F(streets_timing_plan_test, fromJson)
{
    streets_timing_plan::streets_timing_plan timing_plan;
    // Valid JSON
    std::string expected_str = "{\"MsgType\":\"ActiveTimingPlan\",\"TimingPlan\":{\"NoOfPhase\":8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7],\"PedClear\":[0,33,1,43,0,33,0,33],\"MinGreen\":[4,15,4,15,4,15,4,15],\"Passage\":[2.1,5.0,2.0,5.0,2.0,5.0,2.0,5.0],\"MaxGreen\":[37,35,19,40,32,19,29],\"YellowChange\":[3.0,4.0,3.0,3.6,4.0,3.0,3.6],\"RedClear\":[1.0,2.5,1.0,3.40000000000004,1.0,2.5,1.0,3.4],\"PhaseRing\":[1,1,1,1,1,2,2,2,2]}}";
    timing_plan.fromJson(expected_str);
    ASSERT_EQ(8, timing_plan.number_of_phase);
    ASSERT_EQ(timing_plan.TIMING_PLAN_MSG_TYPE, timing_plan.msg_type);
    ASSERT_EQ(8, timing_plan.phase_number_v.size());
    std::stringstream ss;
    for (auto phase_num : timing_plan.phase_number_v)
    {
        ss << phase_num;
    }
    ASSERT_EQ("12345678", ss.str());

    ss.str("");
    for (auto ped_walk : timing_plan.pedestrian_walk_v)
    {
        ss << ped_walk;
    }
    ASSERT_EQ("07070707", ss.str());

    ss.str("");
    for (auto ped_clear : timing_plan.pedestrian_clear_v)
    {
        ss << ped_clear;
    }
    ASSERT_EQ("033143033033", ss.str());

    ss.str("");
    for (auto min_green : timing_plan.min_green_v)
    {
        ss << min_green;
    }
    ASSERT_EQ("415415415415", ss.str());

    ss.str("");
    for (auto passage : timing_plan.passage_v)
    {
        ss << passage << ",";
    }
    ASSERT_EQ("2.1,5,2,5,2,5,2,5,", ss.str());

    ss.str("");
    for (auto max_green : timing_plan.max_green_v)
    {
        ss << max_green;
    }
    ASSERT_EQ("37351940321929", ss.str());

    ss.str("");
    for (auto yellow_change : timing_plan.yellow_change_v)
    {
        ss << yellow_change << ",";
    }
    ASSERT_EQ("3,4,3,3.6,4,3,3.6,", ss.str());

    ss.str("");
    for (auto red_clear : timing_plan.red_clear_v)
    {
        ss << red_clear << ",";
    }
    ASSERT_EQ("1,2.5,1,3.4,1,2.5,1,3.4,", ss.str());

    ss.str("");
    for (auto phase_ring : timing_plan.phase_ring_v)
    {
        ss << phase_ring << ",";
    }
    ASSERT_EQ("1,1,1,1,1,2,2,2,2,", ss.str());

    // Malformatted JSON
    std::string invalid_json = "{invalid}";
    ASSERT_THROW(timing_plan.fromJson(invalid_json), streets_timing_plan_exception);

    // Missing MsgType
    std::string input_json = "{\"MsgType\":\"Missing\"}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing TimingPlan property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\"}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing NoOfPhase property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"invalid\": \"invalid\"}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing PhaseNumber property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing PedWalk property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8,\"PhaseNumber\":[1,2,3,4,5,6,7,8]}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing PedClear property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7]}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing MinGreen property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7],\"PedClear\":[0,33,1,43,0,33,0,33]}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing Passage property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7],\"PedClear\":[0,33,1,43,0,33,0,33],\"MinGreen\":[4,15,4,15,4,15,4,15]}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing MaxGreen property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7],\"PedClear\":[0,33,1,43,0,33,0,33],\"MinGreen\":[4,15,4,15,4,15,4,15],\"Passage\":[2.0,5.0,2.0,5.0,2.0,5.0,2.0,5.0]}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing YellowChange property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7],\"PedClear\":[0,33,1,43,0,33,0,33],\"MinGreen\":[4,15,4,15,4,15,4,15],\"Passage\":[2.0,5.0,2.0,5.0,2.0,5.0,2.0,5.0],\"MaxGreen\":[37,35,19,40,32,19,29]}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing RedClear property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7],\"PedClear\":[0,33,1,43,0,33,0,33],\"MinGreen\":[4,15,4,15,4,15,4,15],\"Passage\":[2.0,5.0,2.0,5.0,2.0,5.0,2.0,5.0],\"MaxGreen\":[37,35,19,40,32,19,29],\"YellowChange\":[3.0,4.0,3.0,3.6,4.0,3.0,3.6]}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);

    // Missing PhaseRing property
    input_json = "{\"MsgType\":\"ActiveTimingPlan\", \"TimingPlan\": {\"NoOfPhase\": 8,\"PhaseNumber\":[1,2,3,4,5,6,7,8],\"PedWalk\":[0,7,0,7,0,7,0,7],\"PedClear\":[0,33,1,43,0,33,0,33],\"MinGreen\":[4,15,4,15,4,15,4,15],\"Passage\":[2.0,5.0,2.0,5.0,2.0,5.0,2.0,5.0],\"MaxGreen\":[37,35,19,40,32,19,29],\"YellowChange\":[3.0,4.0,3.0,3.6,4.0,3.0,3.6],\"RedClear\":[1.0,2.5,1.0,3.40000000000004,1.0,2.5,1.0,3.4]}}";
    ASSERT_THROW(timing_plan.fromJson(input_json), streets_timing_plan_exception);
}