
#include "streets_phase_control_schedule.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <fstream>

using namespace streets_phase_control_schedule;

namespace
{

    class streets_phase_control_schedule_test : public ::testing::Test
    {
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

TEST_F(streets_phase_control_schedule_test, fromJson)
{
   
}