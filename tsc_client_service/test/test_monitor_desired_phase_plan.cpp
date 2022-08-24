#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>
#include "monitor_desired_phase_plan.h"

using testing::_;

namespace traffic_signal_controller_service
{
    class test_monitor_desired_phase_plan : public ::testing::Test
    {
    public:
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_ptr;

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
        }
    };

    TEST(test_monitor_desired_phase_plan, update_desired_phase_plan)
    {
    }

    TEST(test_monitor_desired_phase_plan, add_future_movement_events)
    {
    }

    TEST(test_monitor_desired_phase_plan, process_first_desired_green)
    {
    }

    TEST(test_monitor_desired_phase_plan, process_second_onward_desired_green)
    {
    }

    TEST(test_monitor_desired_phase_plan, populate_movement_event)
    {
    }

    TEST(test_monitor_desired_phase_plan, append_full_green_yellow_red_phases_by_desired_green)
    {
    }
}
