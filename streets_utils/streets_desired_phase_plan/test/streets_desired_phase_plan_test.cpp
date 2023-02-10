
#include "streets_desired_phase_plan.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <fstream>

using namespace streets_desired_phase_plan;

namespace
{

    class streets_desired_phase_plan_test : public ::testing::Test
    {
    protected:
        std::string streets_desired_phase_plan_str = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":1660747993,\"end_time\":1660747998},{\"signal_groups\":[2,6],\"start_time\":1660747993,\"end_time\":1660747998}]}";
        streets_desired_phase_plan::streets_desired_phase_plan streets_desired_pl;
        /**
         * @brief Test Setup method run before each test.
         */
        void SetUp() override
        {
            streets_desired_pl.timestamp = 12121212121;
            streets_desired_phase_plan::signal_group2green_phase_timing sg2gphtimng;
            sg2gphtimng.start_time = 1660747993;
            sg2gphtimng.end_time = 1660747998;
            sg2gphtimng.signal_groups.push_back(1);
            sg2gphtimng.signal_groups.push_back(5);
            streets_desired_pl.desired_phase_plan.push_back(sg2gphtimng);
            sg2gphtimng.signal_groups.clear();
            sg2gphtimng.signal_groups.push_back(2);
            sg2gphtimng.signal_groups.push_back(6);
            streets_desired_pl.desired_phase_plan.push_back(sg2gphtimng);
        }
        /**
         * @brief Test TearDown method run after each test.
         *
         */
        void TearDown() override
        {
        }
    };
};

TEST_F(streets_desired_phase_plan_test, fromJson)
{
    streets_desired_phase_plan::streets_desired_phase_plan local_streets_plan;
    local_streets_plan.fromJson(streets_desired_phase_plan_str);
    ASSERT_EQ(streets_desired_pl.timestamp, local_streets_plan.timestamp);
    ASSERT_EQ(streets_desired_pl.desired_phase_plan.front().end_time, local_streets_plan.desired_phase_plan.front().end_time);
    ASSERT_EQ(streets_desired_pl.desired_phase_plan.front().start_time, local_streets_plan.desired_phase_plan.front().start_time);
    ASSERT_EQ(streets_desired_pl.desired_phase_plan.front().signal_groups.front(), local_streets_plan.desired_phase_plan.front().signal_groups.front());
}

TEST_F(streets_desired_phase_plan_test, toJson)
{
    std::string local_plan_str = streets_desired_pl.toJson();
    ASSERT_EQ(local_plan_str, streets_desired_phase_plan_str);
}
