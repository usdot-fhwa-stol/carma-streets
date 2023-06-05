
#include "streets_phase_control_schedule.h"
#include "streets_phase_control_schedule_exception.h"

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
    streets_phase_control_schedule::streets_phase_control_schedule invalid_schedule;
    std::string input_str = "invalid";
    ASSERT_THROW(invalid_schedule.fromJson(input_str), streets_phase_control_schedule_exception);

    input_str = "{\"MsgType\":\"Invalid\",\"Schedule\":\"Clear\"}";
    ASSERT_THROW(invalid_schedule.fromJson(input_str), streets_phase_control_schedule_exception);

    input_str = "{\"MsgType\":\"Schedule\",\"Schedule\":\"invalid\"}";
    ASSERT_THROW(invalid_schedule.fromJson(input_str), streets_phase_control_schedule_exception);

    streets_phase_control_schedule::streets_phase_control_schedule clear_schedule;
    ASSERT_FALSE(clear_schedule.is_clear_current_schedule);
    input_str = "{\"MsgType\":\"Schedule\",\"Schedule\":\"Clear\"}";
    clear_schedule.fromJson(input_str);
    ASSERT_TRUE(clear_schedule.is_clear_current_schedule);
    ASSERT_EQ(0, clear_schedule.commands.size());
    ASSERT_TRUE(clear_schedule.is_clear_current_schedule);

    streets_phase_control_schedule::streets_phase_control_schedule new_schedule;
    ASSERT_FALSE(new_schedule.is_clear_current_schedule);
    input_str = "{\"MsgType\":\"Schedule\",\"Schedule\":[{\"commandEndTime\":0.0,\"commandPhase\":2,\"commandStartTime\":0.0,\"commandType\":\"hold\"},{\"commandEndTime\":24.0,\"commandPhase\":4,\"commandStartTime\":5.0,\"commandType\":\"hold\"},{\"commandEndTime\":44.0,\"commandPhase\":2,\"commandStartTime\":29.0,\"commandType\":\"hold\"},{\"commandEndTime\":64.0,\"commandPhase\":4,\"commandStartTime\":49.0,\"commandType\":\"hold\"},{\"commandEndTime\":12.0,\"commandPhase\":2,\"commandStartTime\":11.0,\"commandType\":\"forceoff\"},{\"commandEndTime\":69.5,\"commandPhase\":4,\"commandStartTime\":68.5,\"commandType\":\"forceoff\"},{\"commandEndTime\":109.5,\"commandPhase\":2,\"commandStartTime\":108.5,\"commandType\":\"forceoff\"},{\"commandEndTime\":129.5,\"commandPhase\":4,\"commandStartTime\":128.5,\"commandType\":\"forceoff\"},{\"commandEndTime\":23.999767065048218,\"commandPhase\":4,\"commandStartTime\":0.0,\"commandType\":\"call_veh\"},{\"commandEndTime\":133.5,\"commandPhase\":1,\"commandStartTime\":0.0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":133.5,\"commandPhase\":3,\"commandStartTime\":0.0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":0.0,\"commandPhase\":6,\"commandStartTime\":0.0,\"commandType\":\"hold\"},{\"commandEndTime\":24.0,\"commandPhase\":7,\"commandStartTime\":5.0,\"commandType\":\"hold\"},{\"commandEndTime\":44.0,\"commandPhase\":6,\"commandStartTime\":29.0,\"commandType\":\"hold\"},{\"commandEndTime\":64.0,\"commandPhase\":7,\"commandStartTime\":49.0,\"commandType\":\"hold\"},{\"commandEndTime\":12.0,\"commandPhase\":6,\"commandStartTime\":11.0,\"commandType\":\"forceoff\"},{\"commandEndTime\":69.5,\"commandPhase\":7,\"commandStartTime\":68.5,\"commandType\":\"forceoff\"},{\"commandEndTime\":109.5,\"commandPhase\":6,\"commandStartTime\":108.5,\"commandType\":\"forceoff\"},{\"commandEndTime\":129.5,\"commandPhase\":7,\"commandStartTime\":128.5,\"commandType\":\"forceoff\"},{\"commandEndTime\":23.999767065048218,\"commandPhase\":7,\"commandStartTime\":0.0,\"commandType\":\"call_veh\"},{\"commandEndTime\":133.5,\"commandPhase\":5,\"commandStartTime\":0.0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":133.5,\"commandPhase\":8,\"commandStartTime\":0.0,\"commandType\":\"omit_veh\"}]}";
    new_schedule.fromJson(input_str);
    ASSERT_FALSE(new_schedule.is_clear_current_schedule);
    ASSERT_EQ(22, new_schedule.commands.size());
}