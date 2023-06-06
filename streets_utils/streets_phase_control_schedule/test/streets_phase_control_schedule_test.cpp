
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

    streets_phase_control_schedule::streets_phase_control_schedule new_schedule_wrong_data_type;
    input_str = "{\"MsgType\":\"Schedule\",\"Schedule\":[{\"commandEndTime\":0,\"commandPhase\":2,\"commandStartTime\":0.0,\"commandType\":\"hold\"}]}";
    ASSERT_THROW(new_schedule_wrong_data_type.fromJson(input_str), streets_phase_control_schedule_exception);
    ASSERT_FALSE(new_schedule_wrong_data_type.is_clear_current_schedule);
    ASSERT_EQ(0, new_schedule_wrong_data_type.commands.size());

    streets_phase_control_schedule::streets_phase_control_schedule new_schedule;
    ASSERT_FALSE(new_schedule.is_clear_current_schedule);
    input_str = "{\"MsgType\":\"Schedule\",\"Schedule\":[{\"commandEndTime\":0,\"commandPhase\":2,\"commandStartTime\":0,\"commandType\":\"hold\"},{\"commandEndTime\":24,\"commandPhase\":4,\"commandStartTime\":5,\"commandType\":\"hold\"},{\"commandEndTime\":44,\"commandPhase\":2,\"commandStartTime\":29,\"commandType\":\"hold\"},{\"commandEndTime\":64,\"commandPhase\":4,\"commandStartTime\":49,\"commandType\":\"hold\"},{\"commandEndTime\":12,\"commandPhase\":2,\"commandStartTime\":11,\"commandType\":\"forceoff\"},{\"commandEndTime\":69,\"commandPhase\":4,\"commandStartTime\":68,\"commandType\":\"forceoff\"},{\"commandEndTime\":109,\"commandPhase\":2,\"commandStartTime\":108,\"commandType\":\"forceoff\"},{\"commandEndTime\":129,\"commandPhase\":4,\"commandStartTime\":128,\"commandType\":\"forceoff\"},{\"commandEndTime\":23,\"commandPhase\":4,\"commandStartTime\":0,\"commandType\":\"call_veh\"},{\"commandEndTime\":133,\"commandPhase\":1,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":133,\"commandPhase\":3,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":0,\"commandPhase\":6,\"commandStartTime\":0,\"commandType\":\"hold\"},{\"commandEndTime\":24,\"commandPhase\":7,\"commandStartTime\":5,\"commandType\":\"hold\"},{\"commandEndTime\":44,\"commandPhase\":6,\"commandStartTime\":29,\"commandType\":\"hold\"},{\"commandEndTime\":64,\"commandPhase\":7,\"commandStartTime\":49,\"commandType\":\"hold\"},{\"commandEndTime\":12,\"commandPhase\":6,\"commandStartTime\":11,\"commandType\":\"forceoff\"},{\"commandEndTime\":69,\"commandPhase\":7,\"commandStartTime\":68,\"commandType\":\"forceoff\"},{\"commandEndTime\":109,\"commandPhase\":6,\"commandStartTime\":108,\"commandType\":\"forceoff\"},{\"commandEndTime\":129,\"commandPhase\":7,\"commandStartTime\":128,\"commandType\":\"forceoff\"},{\"commandEndTime\":23,\"commandPhase\":7,\"commandStartTime\":0,\"commandType\":\"call_veh\"},{\"commandEndTime\":133,\"commandPhase\":5,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":133,\"commandPhase\":8,\"commandStartTime\":0,\"commandType\":\"omit_veh\"}]}";
    new_schedule.fromJson(input_str);
    ASSERT_FALSE(new_schedule.is_clear_current_schedule);
    ASSERT_EQ(22, new_schedule.commands.size());

    std::string input_str_wrong_data_type = "{\"MsgType\":\"Schedule\",\"Schedule\":[{\"commandEndTime\":0,\"commandPhase\":2,\"commandStartTime\":0,\"commandType\":\"hold\"},{\"commandEndTime\":0.0,\"commandPhase\":2,\"commandStartTime\":0.0,\"commandType\":\"hold\"}]}";
    ASSERT_THROW(new_schedule.fromJson(input_str_wrong_data_type), streets_phase_control_schedule_exception);
    ASSERT_EQ(22, new_schedule.commands.size());
    
    input_str = "{\"MsgType\":\"Schedule\",\"Schedule\":[{\"commandEndTime\":0,\"commandPhase\":2,\"commandStartTime\":0,\"commandType\":\"hold\"}]}";
    new_schedule.fromJson(input_str);
    ASSERT_EQ(1, new_schedule.commands.size());
    ASSERT_FALSE(new_schedule.is_clear_current_schedule);

    input_str = "{\"MsgType\":\"Schedule\",\"Schedule\":\"Clear\"}";
    new_schedule.fromJson(input_str);
    ASSERT_EQ(0, new_schedule.commands.size());
    ASSERT_TRUE(new_schedule.is_clear_current_schedule);

    ASSERT_THROW(new_schedule.fromJson(input_str_wrong_data_type), streets_phase_control_schedule_exception);
    ASSERT_EQ(0, new_schedule.commands.size());
    ASSERT_TRUE(new_schedule.is_clear_current_schedule);
}