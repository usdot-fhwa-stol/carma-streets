
#include "streets_phase_control_command.h"
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

    class streets_phase_control_command_test : public ::testing::Test
    {
    };
};

TEST_F(streets_phase_control_command_test, set_command_type)
{
    streets_phase_control_command command1;
    command1.set_command_type("call_veh");
    ASSERT_EQ(COMMAND_TYPE::CALL_VEH_PHASES, command1.command_type);
    command1.set_command_type("call_ped");
    ASSERT_EQ(COMMAND_TYPE::CALL_PED_PHASES, command1.command_type);
    command1.set_command_type("forceoff");
    ASSERT_EQ(COMMAND_TYPE::FORCEOFF_PHASES, command1.command_type);
    command1.set_command_type("hold");
    ASSERT_EQ(COMMAND_TYPE::HOLD_VEH_PHASES, command1.command_type);
    command1.set_command_type("omit_veh");
    ASSERT_EQ(COMMAND_TYPE::OMIT_VEH_PHASES, command1.command_type);
    command1.set_command_type("omit_ped");
    ASSERT_EQ(COMMAND_TYPE::OMIT_PED_PHASES, command1.command_type);
    ASSERT_THROW(command1.set_command_type("unkown_command"), streets_phase_control_schedule_exception);

    streets_phase_control_command command2("call_veh", 2, 0.0, 23.999767065048218);
    ASSERT_EQ(COMMAND_TYPE::CALL_VEH_PHASES, command2.command_type);
}