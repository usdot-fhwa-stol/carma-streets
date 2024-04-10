
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
    std::stringstream out;
    std::string expected_str;
    command1.set_command_type("call_veh");
    ASSERT_EQ(COMMAND_TYPE::CALL_VEH_PHASES, command1.command_type);
    out.str("");
    out << command1;
    expected_str = "Command type: call_veh, command phase: 0, start time: 0, end time: 0";
    ASSERT_EQ(expected_str, out.str());

    command1.set_command_type("call_ped");
    ASSERT_EQ(COMMAND_TYPE::CALL_PED_PHASES, command1.command_type);
    out.str("");
    out << command1;
    expected_str = "Command type: call_ped, command phase: 0, start time: 0, end time: 0";
    ASSERT_EQ(expected_str, out.str());

    command1.set_command_type("forceoff");
    ASSERT_EQ(COMMAND_TYPE::FORCEOFF_PHASES, command1.command_type);
    out.str("");
    out << command1;
    expected_str = "Command type: forceoff, command phase: 0, start time: 0, end time: 0";
    ASSERT_EQ(expected_str, out.str());

    command1.set_command_type("hold");
    ASSERT_EQ(COMMAND_TYPE::HOLD_VEH_PHASES, command1.command_type);
    out.str("");
    out << command1;
    expected_str = "Command type: hold, command phase: 0, start time: 0, end time: 0";
    ASSERT_EQ(expected_str, out.str());

    command1.set_command_type("omit_veh");
    ASSERT_EQ(COMMAND_TYPE::OMIT_VEH_PHASES, command1.command_type);
    out.str("");
    out << command1;
    expected_str = "Command type: omit_veh, command phase: 0, start time: 0, end time: 0";
    ASSERT_EQ(expected_str, out.str());

    command1.set_command_type("omit_ped");
    ASSERT_EQ(COMMAND_TYPE::OMIT_PED_PHASES, command1.command_type);
    out.str("");
    out << command1;
    expected_str = "Command type: omit_ped, command phase: 0, start time: 0, end time: 0";
    ASSERT_EQ(expected_str, out.str());

    ASSERT_THROW(command1.set_command_type("unkown_command"), streets_phase_control_schedule_exception);

    streets_phase_control_command command2("call_veh", 2, 0.0, 23.999767065048218);
    ASSERT_EQ(COMMAND_TYPE::CALL_VEH_PHASES, command2.command_type);
    expected_str = "Command type: call_veh, command phase: 2, start time: 0, end time: 23";
    out.str("");
    out << command2;
    ASSERT_EQ(expected_str, out.str());
}

TEST_F(streets_phase_control_command_test, COMMAND_TYPE_to_string)
{
    streets_phase_control_command command1;
    std::string expected_str = "call_veh";
    ASSERT_EQ(expected_str, command1.COMMAND_TYPE_to_string(COMMAND_TYPE::CALL_VEH_PHASES));

    expected_str = "call_ped";
    ASSERT_EQ(expected_str, command1.COMMAND_TYPE_to_string(COMMAND_TYPE::CALL_PED_PHASES));

    expected_str = "forceoff";
    ASSERT_EQ(expected_str, command1.COMMAND_TYPE_to_string(COMMAND_TYPE::FORCEOFF_PHASES));

    expected_str = "hold";
    ASSERT_EQ(expected_str, command1.COMMAND_TYPE_to_string(COMMAND_TYPE::HOLD_VEH_PHASES));

    expected_str = "omit_veh";
    ASSERT_EQ(expected_str, command1.COMMAND_TYPE_to_string(COMMAND_TYPE::OMIT_VEH_PHASES));

    expected_str = "omit_ped";
    ASSERT_EQ(expected_str, command1.COMMAND_TYPE_to_string(COMMAND_TYPE::OMIT_PED_PHASES));

    expected_str = "call_veh";
    ASSERT_EQ(expected_str, command1.COMMAND_TYPE_to_string(COMMAND_TYPE::CALL_VEH_PHASES));
}