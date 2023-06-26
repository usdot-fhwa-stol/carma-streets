
#include "streets_snmp_cmd.h"
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

    class streets_snmp_cmd_test : public ::testing::Test
    {
    };

    TEST_F(streets_snmp_cmd_test, get_cmd_info)
    {
        uint64_t start_time = 1686683893;
        PHASE_CONTROL_TYPE type = PHASE_CONTROL_TYPE::HOLD_VEH_PHASES;
        int64_t val = 2;
        snmp_cmd_struct snmp_cmd1(start_time, type, val);
        std::string expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:1686683893; value_set:2";
        ASSERT_EQ(expected_str, snmp_cmd1.get_cmd_info());

        type = PHASE_CONTROL_TYPE::CALL_PED_PHASES;
        snmp_cmd_struct snmp_cmd2(start_time, type, val);
        expected_str = "control_cmd_type:Pedestrian Call; execution_start_time:1686683893; value_set:2";
        ASSERT_EQ(expected_str, snmp_cmd2.get_cmd_info());

        type = PHASE_CONTROL_TYPE::CALL_VEH_PHASES;
        snmp_cmd_struct snmp_cmd3(start_time, type, val);
        expected_str = "control_cmd_type:Vehicle Call; execution_start_time:1686683893; value_set:2";
        ASSERT_EQ(expected_str, snmp_cmd3.get_cmd_info());

        type = PHASE_CONTROL_TYPE::OMIT_PED_PHASES;
        snmp_cmd_struct snmp_cmd4(start_time, type, val);
        expected_str = "control_cmd_type:Pedestrian Omit; execution_start_time:1686683893; value_set:2";
        ASSERT_EQ(expected_str, snmp_cmd4.get_cmd_info());

        type = PHASE_CONTROL_TYPE::OMIT_VEH_PHASES;
        snmp_cmd_struct snmp_cmd5(start_time, type, val);
        expected_str = "control_cmd_type:Vehicle Omit; execution_start_time:1686683893; value_set:2";
        ASSERT_EQ(expected_str, snmp_cmd5.get_cmd_info());

        type = PHASE_CONTROL_TYPE::FORCEOFF_PHASES;
        snmp_cmd_struct snmp_cmd6(start_time, type, val);
        expected_str = "control_cmd_type:Vehicle Forceoff; execution_start_time:1686683893; value_set:2";
        ASSERT_EQ(expected_str, snmp_cmd6.get_cmd_info());
    }
};