#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>
#include "monitor_desired_phase_plan.h"

namespace traffic_signal_controller_service
{

    class test_monitor_desired_phase_plan : public ::testing::Test
    {
    public:
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_ptr;
        std::shared_ptr<monitor_desired_phase_plan> monitor_dpp_ptr;
        std::shared_ptr<tsc_state> tsc_state_ptr;

    protected:
        void SetUp() override
        {
            // Create mock spat
            spat_msg_ptr = std::make_shared<signal_phase_and_timing::spat>();

            signal_phase_and_timing::intersection_state intersection_state;

            signal_phase_and_timing::movement_state state_1;
            state_1.signal_group = 1;
            signal_phase_and_timing::movement_event event_1;
            event_1.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_1.timing.start_time = 1000;
            event_1.timing.min_end_time = 1100;
            state_1.state_time_speed.push_back(event_1);
            intersection_state.states.push_back(state_1);

            signal_phase_and_timing::movement_state state_2;
            state_2.signal_group = 2;
            signal_phase_and_timing::movement_event event_2;
            event_2.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_2.timing.start_time = 20;
            state_2.state_time_speed.push_back(event_2);
            intersection_state.states.push_back(state_2);

            signal_phase_and_timing::movement_state state_3;
            state_3.signal_group = 3;
            signal_phase_and_timing::movement_event event_3;
            event_3.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; // Green
            event_3.timing.start_time = 30;
            state_3.state_time_speed.push_back(event_3);
            intersection_state.states.push_back(state_3);

            signal_phase_and_timing::movement_state state_4;
            state_4.signal_group = 4;
            signal_phase_and_timing::movement_event event_4;
            event_4.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_4.timing.start_time = 30;
            state_4.state_time_speed.push_back(event_4);
            intersection_state.states.push_back(state_4);

            signal_phase_and_timing::movement_state state_5;
            state_5.signal_group = 5;
            signal_phase_and_timing::movement_event event_5;
            event_5.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_5.timing.start_time = 30;
            state_5.state_time_speed.push_back(event_5);
            intersection_state.states.push_back(state_5);

            signal_phase_and_timing::movement_state state_6;
            state_6.signal_group = 6;
            signal_phase_and_timing::movement_event event_6;
            event_6.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_6.timing.start_time = 30;
            state_6.state_time_speed.push_back(event_6);
            intersection_state.states.push_back(state_6);

            signal_phase_and_timing::movement_state state_7;
            state_7.signal_group = 7;
            signal_phase_and_timing::movement_event event_7;
            event_7.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; // Green
            event_7.timing.start_time = 30;
            state_7.state_time_speed.push_back(event_7);
            intersection_state.states.push_back(state_7);

            signal_phase_and_timing::movement_state state_8;
            state_8.signal_group = 8;
            signal_phase_and_timing::movement_event event_8;
            event_8.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_8.timing.start_time = 30;
            state_8.state_time_speed.push_back(event_8);
            intersection_state.states.push_back(state_8);
            spat_msg_ptr->intersections.push_back(intersection_state);

            // mock tsc_state
            std::string dummy_ip = "192.168.10.10";
            int dummy_port = 601;
            snmp_client mock_client_worker(dummy_ip, dummy_port);
            auto unique_client = std::make_unique<snmp_client>(mock_client_worker);
            tsc_state_ptr = std::make_shared<tsc_state>(std::move(unique_client));
            signal_group_state phase_1_state;
            phase_1_state.signal_group_id = 1;
            phase_1_state.phase_num = 1;
            phase_1_state.min_green = 1000;
            phase_1_state.max_green = 2000;
            phase_1_state.green_duration = 1000;
            phase_1_state.yellow_duration = 1000;
            phase_1_state.red_clearance = 1000;
            phase_1_state.red_duration = 10000;
            // tsc_state_ptr->signal_group_state_map_.insert({1, phase_1_state});
        }
    };

    TEST_F(test_monitor_desired_phase_plan, update_desired_phase_plan)
    {
        monitor_dpp_ptr = std::make_shared<monitor_desired_phase_plan>();
        ASSERT_TRUE(monitor_dpp_ptr->get_desired_phase_plan_ptr() == nullptr);
        std::string streets_desired_phase_plan_str = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":1660747993,\"end_time\":1660757998},{\"signal_groups\":[2,6],\"start_time\":1660749993,\"end_time\":1660749098},{\"signal_groups\":[3,7],\"start_time\":1660750993,\"end_time\":1660750998},{\"signal_groups\":[4,8],\"start_time\":1660757993,\"end_time\":1660757998}]}";
        monitor_dpp_ptr->update_desired_phase_plan(streets_desired_phase_plan_str);
        ASSERT_EQ(12121212121, monitor_dpp_ptr->get_desired_phase_plan_ptr()->timestamp);
        ASSERT_EQ(4, monitor_dpp_ptr->get_desired_phase_plan_ptr()->desired_phase_plan.size());
        ASSERT_EQ(1660747993, monitor_dpp_ptr->get_desired_phase_plan_ptr()->desired_phase_plan.front().start_time);
        ASSERT_EQ(1660757998, monitor_dpp_ptr->get_desired_phase_plan_ptr()->desired_phase_plan.front().end_time);
        ASSERT_EQ(1, monitor_dpp_ptr->get_desired_phase_plan_ptr()->desired_phase_plan.front().signal_groups.front());
        ASSERT_EQ(5, monitor_dpp_ptr->get_desired_phase_plan_ptr()->desired_phase_plan.front().signal_groups.back());
    }

    TEST_F(test_monitor_desired_phase_plan, update_spat_future_movement_events)
    {
        monitor_dpp_ptr = std::make_shared<monitor_desired_phase_plan>();
        std::string streets_desired_phase_plan_str = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":1660747993,\"end_time\":1660757998},{\"signal_groups\":[2,6],\"start_time\":1660749993,\"end_time\":1660749098},{\"signal_groups\":[3,7],\"start_time\":1660750993,\"end_time\":1660750998},{\"signal_groups\":[4,8],\"start_time\":1660757993,\"end_time\":1660757998}]}";
        monitor_dpp_ptr->update_desired_phase_plan(streets_desired_phase_plan_str);

        // Current spat should only contain the ONLY one current movement event for each movement state.
        for (auto movement_state : spat_msg_ptr->intersections.front().states)
        {
            ASSERT_EQ(8, spat_msg_ptr->intersections.front().states.size());
            ASSERT_EQ(1, movement_state.state_time_speed.size());
        }

        monitor_dpp_ptr->update_spat_future_movement_events(spat_msg_ptr, tsc_state_ptr);

        for (auto movement_state : spat_msg_ptr->intersections.front().states)
        {
            int sg = (int)movement_state.signal_group;
            std::cout << "\n\n signal group = " << sg << std::endl;
            // ASSERT_NE(1, movement_state.state_time_speed.size());
            std::cout << "\t phase_state \t start_time \t min_end_time\n";
            for (auto movement_event : movement_state.state_time_speed)
            {
                std::string state_name = "";
                switch (movement_event.event_state)
                {
                case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                    state_name = "red";
                    break;
                case signal_phase_and_timing::movement_phase_state::protected_clearance:
                    state_name = "yellow";
                    break;
                case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                    state_name = "green";
                    break;

                default:
                    break;
                }
                std::cout << "\t " << state_name << " \t " << movement_event.timing.start_time << " \t\t\t " << movement_event.timing.min_end_time << "\n";
            }
        }
    }

    TEST_F(test_monitor_desired_phase_plan, process_first_desired_green)
    {
    }

    TEST_F(test_monitor_desired_phase_plan, process_second_onward_desired_green)
    {
    }

    TEST_F(test_monitor_desired_phase_plan, populate_movement_event)
    {
    }

    TEST_F(test_monitor_desired_phase_plan, append_full_green_yellow_red_phases_by_desired_green)
    {
    }
}
