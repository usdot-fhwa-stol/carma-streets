
#include "streets_desired_phase_plan_arbitrator.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <fstream>

using namespace streets_desired_phase_plan_arbitrator;

namespace
{

    class streets_desired_phase_plan_arbitrator_test : public ::testing::Test
    {
    protected:
        std::string streets_desired_phase_plan_arbitrator_str = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":1660747993,\"end_time\":1660747998},{\"signal_groups\":[2,6],\"start_time\":1660747993,\"end_time\":1660747998}]}";
        streets_desired_phase_plan_arbitrator::streets_desired_phase_plan_arbitrator streets_desired_pl;
        /**
         * @brief Test Setup method run before each test.
         */
        void SetUp() override
        {
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

TEST_F(streets_desired_phase_plan_arbitrator_test, fromJson)
{
}

TEST_F(streets_desired_phase_plan_arbitrator_test, toJson)
{
}
