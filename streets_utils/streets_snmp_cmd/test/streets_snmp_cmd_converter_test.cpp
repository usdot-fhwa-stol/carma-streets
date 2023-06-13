
#include "streets_snmp_cmd_converter.h"
#include "streets_snmp_cmd_exception.h"

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <fstream>

using namespace streets_snmp_cmd;

namespace
{
    class streets_snmp_cmd_converter_test : public ::testing::Test
    {
    public:
        streets_snmp_cmd_converter converter;
    };

    TEST_F(streets_snmp_cmd_converter_test, to_phase_control_type)
    {
        ASSERT_EQ(streets_snmp_cmd::PHASE_CONTROL_TYPE::CALL_PED_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::CALL_PED_PHASES));
        ASSERT_EQ(streets_snmp_cmd::PHASE_CONTROL_TYPE::CALL_VEH_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::CALL_VEH_PHASES));
        ASSERT_EQ(streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_PED_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::OMIT_PED_PHASES));
        ASSERT_EQ(streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::OMIT_VEH_PHASES));
        ASSERT_EQ(streets_snmp_cmd::PHASE_CONTROL_TYPE::FORCEOFF_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::FORCEOFF_PHASES));
        ASSERT_EQ(streets_snmp_cmd::PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::HOLD_VEH_PHASES));
    };

    TEST_F(streets_snmp_cmd_converter_test, create_snmp_command)
    {
    };

    TEST_F(streets_snmp_cmd_converter_test, create_snmp_cmds_by_phase_control_schedule)
    {
    }

};
