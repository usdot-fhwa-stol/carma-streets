
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

    TEST_F(streets_snmp_cmd_converter_test, create_snmp_command_by_phases)
    {
        std::vector<int> phases;
        phases.push_back(7);
        uint64_t start_time = 1686751639;
        PHASE_CONTROL_TYPE control_type;
        auto command1 = converter.create_snmp_command_by_phases(phases, control_type, start_time);
        std::string expected_str = "control_cmd_type:; execution_start_time:1686751639; value_set:64";
        ASSERT_EQ(expected_str, command1.get_cmd_info());

        phases.push_back(1);
        auto command2 = converter.create_snmp_command_by_phases(phases, control_type, start_time);
        expected_str = "control_cmd_type:; execution_start_time:1686751639; value_set:65";
        ASSERT_EQ(expected_str, command2.get_cmd_info());

        phases.push_back(3);
        auto command3 = converter.create_snmp_command_by_phases(phases, control_type, start_time);
        expected_str = "control_cmd_type:; execution_start_time:1686751639; value_set:69";
        ASSERT_EQ(expected_str, command3.get_cmd_info());

        phases.push_back(8);
        auto command4 = converter.create_snmp_command_by_phases(phases, control_type, start_time);
        expected_str = "control_cmd_type:; execution_start_time:1686751639; value_set:197";
        ASSERT_EQ(expected_str, command4.get_cmd_info());
    };

    TEST_F(streets_snmp_cmd_converter_test, create_snmp_reset_command)
    {
        uint64_t start_time = 1686751639;
        PHASE_CONTROL_TYPE control_type;
        auto command = converter.create_snmp_reset_command(control_type, start_time);
        std::string expected_str = "control_cmd_type:; execution_start_time:1686751639; value_set:0";
        ASSERT_EQ(expected_str, command.get_cmd_info());
    };

    TEST_F(streets_snmp_cmd_converter_test, create_snmp_cmds_by_phase_control_schedule)
    {
        // Different commandType, same phase and start/end time the same
        auto pcs = std::make_shared<streets_phase_control_schedule::streets_phase_control_schedule>();
        streets_phase_control_schedule::streets_phase_control_command psc_cmd0("hold", 2, 0, 0);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd01("omit_ped", 2, 0, 0);
        pcs->commands.push_back(psc_cmd0);
        pcs->commands.push_back(psc_cmd01);

        // Same commandType, two different phases and different start time, but end time is the same
        streets_phase_control_schedule::streets_phase_control_command psc_cmd2("hold", 4, 6, 24);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd3("hold", 2, 5, 24);
        pcs->commands.push_back(psc_cmd2);
        pcs->commands.push_back(psc_cmd3);

        // Same commandType, two different phases and same start time, and end time
        streets_phase_control_schedule::streets_phase_control_command psc_cmd4("hold", 4, 25, 28);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd5("hold", 2, 25, 28);
        pcs->commands.push_back(psc_cmd4);
        pcs->commands.push_back(psc_cmd5);

        // Same commandType, two different phases and different start/end time
        streets_phase_control_schedule::streets_phase_control_command psc_cmd6("hold", 2, 48, 65);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd7("hold", 4, 49, 64);
        pcs->commands.push_back(psc_cmd6);
        pcs->commands.push_back(psc_cmd7);

        streets_phase_control_schedule::streets_phase_control_command psc_cmd8("forceoff", 4, 49, 69);
        pcs->commands.push_back(psc_cmd8);

        // Same commandType, two difference phases and start/end time are the same.
        streets_phase_control_schedule::streets_phase_control_command psc_cmd9("omit_veh", 5, 0, 135);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd10("omit_veh", 8, 0, 135);
        pcs->commands.push_back(psc_cmd9);
        pcs->commands.push_back(psc_cmd10);

        // Different commandType and phases, start and end time are the same
        streets_phase_control_schedule::streets_phase_control_command psc_cmd13("call_ped", 8, 158, 159);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd14("call_veh", 5, 158, 159);
        pcs->commands.push_back(psc_cmd13);
        pcs->commands.push_back(psc_cmd14);

        streets_phase_control_schedule::streets_phase_control_command psc_cmd15("hold", 1, 160, 200);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd16("hold", 2, 161, 199);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd17("hold", 3, 162, 198);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd18("hold", 4, 163, 197);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd19("hold", 5, 164, 196);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd20("hold", 6, 165, 195);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd21("hold", 7, 166, 194);
        streets_phase_control_schedule::streets_phase_control_command psc_cmd22("hold", 8, 167, 193);
        pcs->commands.push_back(psc_cmd15);
        pcs->commands.push_back(psc_cmd16);
        pcs->commands.push_back(psc_cmd17);
        pcs->commands.push_back(psc_cmd18);
        pcs->commands.push_back(psc_cmd19);
        pcs->commands.push_back(psc_cmd20);
        pcs->commands.push_back(psc_cmd21);

        auto cmds_result = converter.create_snmp_cmds_by_phase_control_schedule(pcs);
        std::string expected_str = "";
        while(!cmds_result.empty())
        {
            auto snmp_cmd = cmds_result.front();
            cmds_result.pop();
            SPDLOG_INFO("{0}", snmp_cmd.get_cmd_info());
            if (snmp_cmd.start_time_ == 0)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:0; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
                else if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Omit; execution_start_time:0; value_set:144";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
                else if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::OMIT_PED_PHASES)
                {
                    expected_str = "control_cmd_type:Pedestrian Omit; execution_start_time:0; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 5)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:5; value_set:2";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 6)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:6; value_set:10";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 24)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:24; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 25)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:25; value_set:10";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 28)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:28; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 48)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:48; value_set:2";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 49)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:49; value_set:10";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
                else if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Forceoff; execution_start_time:49; value_set:8";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 64)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:64; value_set:2";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 65)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:65; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 69)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::FORCEOFF_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Forceoff; execution_start_time:69; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 135)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Omit; execution_start_time:135; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 158)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::CALL_PED_PHASES)
                {
                    expected_str = "control_cmd_type:Pedestrian Call; execution_start_time:158; value_set:128";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
                else if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Call; execution_start_time:158; value_set:16";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 159)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::CALL_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Call; execution_start_time:159; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
                else if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::OMIT_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Omit; execution_start_time:159; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 160)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:160; value_set:1";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 161)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:161; value_set:3";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 162)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:162; value_set:7";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 163)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:163; value_set:15";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 164)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:164; value_set:31";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 165)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:165; value_set:63";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 166)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:166; value_set:127";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 194)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:194; value_set:63";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 195)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:195; value_set:31";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 196)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:196; value_set:15";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 197)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:197; value_set:7";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 198)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:198; value_set:3";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 199)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:199; value_set:1";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
            else if (snmp_cmd.start_time_ == 200)
            {
                if (snmp_cmd.control_type_ == PHASE_CONTROL_TYPE::HOLD_VEH_PHASES)
                {
                    expected_str = "control_cmd_type:Vehicle Hold; execution_start_time:200; value_set:0";
                    ASSERT_EQ(expected_str, snmp_cmd.get_cmd_info());
                }
            }
        }
    
    }

    TEST_F(streets_snmp_cmd_converter_test, to_phase_control_type)
    {
        ASSERT_EQ(PHASE_CONTROL_TYPE::CALL_PED_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::CALL_PED_PHASES));
        ASSERT_EQ(PHASE_CONTROL_TYPE::CALL_VEH_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::CALL_VEH_PHASES));
        ASSERT_EQ(PHASE_CONTROL_TYPE::OMIT_PED_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::OMIT_PED_PHASES));
        ASSERT_EQ(PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::OMIT_VEH_PHASES));
        ASSERT_EQ(PHASE_CONTROL_TYPE::FORCEOFF_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::FORCEOFF_PHASES));
        ASSERT_EQ(PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, converter.to_phase_control_type(streets_phase_control_schedule::COMMAND_TYPE::HOLD_VEH_PHASES));
    }
};
