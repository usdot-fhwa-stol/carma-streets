#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>
#include "monitor_desired_phase_plan.h"
#include "monitor_desired_phase_plan_exception.h"
#include <spdlog/spdlog.h>
#include <chrono>

namespace traffic_signal_controller_service
{

    class test_monitor_desired_phase_plan : public ::testing::Test
    {
    public:
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_two_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_three_ptr;
        std::shared_ptr<monitor_desired_phase_plan> monitor_dpp_ptr;
        std::shared_ptr<tsc_state> tsc_state_ptr;
        uint16_t current_hour_in_tenths_secs;

    protected:
        void SetUp() override
        {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
            uint64_t epoch_timestamp = epochMs.count();
            auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(now.time_since_epoch()).count();
            current_hour_in_tenths_secs = (epoch_timestamp - hours_since_epoch * 3600 * 1000) / 100;

            // Create mock spat
            spat_msg_ptr = std::make_shared<signal_phase_and_timing::spat>();
            spat_msg_two_ptr = std::make_shared<signal_phase_and_timing::spat>();
            spat_msg_three_ptr = std::make_shared<signal_phase_and_timing::spat>();

            signal_phase_and_timing::intersection_state intersection_state;
            signal_phase_and_timing::intersection_state intersection_state_two;
            signal_phase_and_timing::intersection_state intersection_state_three;

            signal_phase_and_timing::movement_state state_1;
            state_1.signal_group = 1;
            signal_phase_and_timing::movement_event event_1;
            event_1.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; // Green
            event_1.timing.start_time = current_hour_in_tenths_secs;
            event_1.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_1.state_time_speed.push_back(event_1);
            intersection_state.states.push_back(state_1);

            state_1.state_time_speed.front().event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            intersection_state_two.states.push_back(state_1);
            intersection_state_three.states.push_back(state_1);

            signal_phase_and_timing::movement_state state_2;
            state_2.signal_group = 2;
            signal_phase_and_timing::movement_event event_2;
            event_2.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_2.timing.start_time = current_hour_in_tenths_secs;
            event_2.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_2.state_time_speed.push_back(event_2);
            intersection_state.states.push_back(state_2);
            intersection_state_three.states.push_back(state_2);

            state_2.state_time_speed.front().event_state = signal_phase_and_timing::movement_phase_state::protected_clearance; // Yellow
            intersection_state_two.states.push_back(state_2);

            signal_phase_and_timing::movement_state state_3;
            state_3.signal_group = 3;
            signal_phase_and_timing::movement_event event_3;
            event_3.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_3.timing.start_time = current_hour_in_tenths_secs;
            event_3.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_3.state_time_speed.push_back(event_3);
            intersection_state.states.push_back(state_3);
            intersection_state_two.states.push_back(state_3);
            intersection_state_three.states.push_back(state_3);

            signal_phase_and_timing::movement_state state_4;
            state_4.signal_group = 4;
            signal_phase_and_timing::movement_event event_4;
            event_4.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_4.timing.start_time = current_hour_in_tenths_secs;
            event_4.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_4.state_time_speed.push_back(event_4);
            intersection_state.states.push_back(state_4);
            intersection_state_two.states.push_back(state_4);
            intersection_state_three.states.push_back(state_4);

            signal_phase_and_timing::movement_state state_5;
            state_5.signal_group = 5;
            signal_phase_and_timing::movement_event event_5;
            event_5.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; // Green
            event_5.timing.start_time = current_hour_in_tenths_secs;
            event_5.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_5.state_time_speed.push_back(event_5);
            intersection_state.states.push_back(state_5);

            state_5.state_time_speed.front().event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            intersection_state_two.states.push_back(state_5);
            intersection_state_three.states.push_back(state_5);

            signal_phase_and_timing::movement_state state_6;
            state_6.signal_group = 6;
            signal_phase_and_timing::movement_event event_6;
            event_6.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_6.timing.start_time = current_hour_in_tenths_secs;
            event_6.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_6.state_time_speed.push_back(event_6);
            intersection_state.states.push_back(state_6);
            intersection_state_three.states.push_back(state_6);

            state_6.state_time_speed.front().event_state = signal_phase_and_timing::movement_phase_state::protected_clearance; // Yellow
            intersection_state_two.states.push_back(state_6);

            signal_phase_and_timing::movement_state state_7;
            state_7.signal_group = 7;
            signal_phase_and_timing::movement_event event_7;
            event_7.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // RED
            event_7.timing.start_time = current_hour_in_tenths_secs;
            event_7.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_7.state_time_speed.push_back(event_7);
            intersection_state.states.push_back(state_7);
            intersection_state_two.states.push_back(state_7);
            intersection_state_three.states.push_back(state_7);

            signal_phase_and_timing::movement_state state_8;
            state_8.signal_group = 8;
            signal_phase_and_timing::movement_event event_8;
            event_8.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; // Red
            event_8.timing.start_time = current_hour_in_tenths_secs;
            event_8.timing.min_end_time = current_hour_in_tenths_secs + 10;
            state_8.state_time_speed.push_back(event_8);
            intersection_state.states.push_back(state_8);
            intersection_state_two.states.push_back(state_8);
            intersection_state_three.states.push_back(state_8);

            spat_msg_ptr->set_intersection(intersection_state);
            spat_msg_two_ptr->set_intersection(intersection_state_two);
            spat_msg_three_ptr->set_intersection(intersection_state_three);

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
        // Add the future movement events without a valid spat
        try{
            monitor_dpp_ptr->update_spat_future_movement_events(nullptr, nullptr);
        }
        catch( const monitor_desired_phase_plan_exception &e ) {
            ASSERT_STREQ(e.what(), "SPAT and TSC state pointers cannot be null. SKIP prcessing!");
        }
        auto invalid_spat_msg_ptr = std::make_shared<signal_phase_and_timing::spat>();
        try {
            monitor_dpp_ptr->update_spat_future_movement_events(invalid_spat_msg_ptr, tsc_state_ptr);
        }
        catch( const signal_phase_and_timing::signal_phase_and_timing_exception &e ) {
            ASSERT_STREQ(e.what(), "No intersection included currently in SPaT!");
        }
        ASSERT_THROW(invalid_spat_msg_ptr->get_intersection(), signal_phase_and_timing::signal_phase_and_timing_exception);
        signal_phase_and_timing::intersection_state intersection;
        invalid_spat_msg_ptr->set_intersection(intersection);
        try {
            monitor_dpp_ptr->update_spat_future_movement_events(invalid_spat_msg_ptr, tsc_state_ptr);
        }
        catch( const monitor_desired_phase_plan_exception &e) {
            ASSERT_STREQ(e.what(), "Intersections states cannot be empty!");
        }
        ASSERT_TRUE(invalid_spat_msg_ptr->get_intersection().states.empty());

        // Current spat should only contain the ONLY one current movement event for each movement state.
        for (auto movement_state : spat_msg_ptr->get_intersection().states)
        {
            ASSERT_EQ(8, spat_msg_ptr->get_intersection().states.size());
            ASSERT_EQ(1, movement_state.state_time_speed.size());
        }

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::milliseconds epochMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        uint64_t epoch_timestamp = epochMs.count();
        monitor_dpp_ptr = std::make_shared<monitor_desired_phase_plan>();
       
        // Add future movement events with an EMPTY desired phase plan, and it should not modify the SPAT message
        try {
            monitor_dpp_ptr->update_spat_future_movement_events(spat_msg_ptr, tsc_state_ptr);
        }
        catch (const monitor_desired_phase_plan_exception &e) {
            ASSERT_STREQ(e.what(), "Desired phase plan is empty. No update.");
        }
        for (auto movement_state : spat_msg_ptr->get_intersection().states)
        {
            ASSERT_EQ(8, spat_msg_ptr->get_intersection().states.size());
            ASSERT_EQ(1, movement_state.state_time_speed.size());
        }

        // Add future movement events with an INVALID desired phase plan, and it should not modify the SPAT message
        std::string streets_desired_phase_plan_invalid_str = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "}]}";
        monitor_dpp_ptr->update_desired_phase_plan(streets_desired_phase_plan_invalid_str);
        try {
            monitor_dpp_ptr->update_spat_future_movement_events(spat_msg_ptr, tsc_state_ptr);
        }
        catch (const signal_phase_and_timing::signal_phase_and_timing_exception &e) {
            ASSERT_STREQ(e.what(), "Desired phase plan signal group ids list is empty. No update.");
        }
        for (auto movement_state : spat_msg_ptr->get_intersection().states)
        {
            ASSERT_EQ(8, spat_msg_ptr->get_intersection().states.size());
            ASSERT_EQ(1, movement_state.state_time_speed.size());
        }

        // Process valid desired phase plan and update the desired phase plan for TSC service
        std::string streets_desired_phase_plan_str = "{\"timestamp\":12121212121,\"desired_phase_plan\":[{\"signal_groups\":[1,5],\"start_time\":" + std::to_string(epoch_timestamp) + ",\"end_time\":" + std::to_string(epoch_timestamp + 10000) + "},{\"signal_groups\":[2,6],\"start_time\":" + std::to_string(epoch_timestamp + 10000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 20000) + "},{\"signal_groups\":[3,7],\"start_time\":" + std::to_string(epoch_timestamp + 20000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 30000) + "},{\"signal_groups\":[4,8],\"start_time\":" + std::to_string(epoch_timestamp + 30000) + ",\"end_time\":" + std::to_string(epoch_timestamp + 40000) + "}]}";
        monitor_dpp_ptr->update_desired_phase_plan(streets_desired_phase_plan_str);

        /****
         * START: Test Scenario one:  There are two green phases [1,5] in the current SPAT movement event.
         * ***/
        // Add future movement events
        monitor_dpp_ptr->update_spat_future_movement_events(spat_msg_ptr, tsc_state_ptr);
        for (auto movement_state : spat_msg_ptr->get_intersection().states)
        {
            int sg = (int)movement_state.signal_group;
            SPDLOG_DEBUG("\n");
            if (sg == 1 || sg == 5)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 30 secs
                        }

                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 10 secs
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 2 || sg == 6)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 20 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 10 secs, but it starts with second desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 3)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red.
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 10 secs, but it starts with third desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 4 || sg == 8)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs, but it starts with fourth desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
        }
        /****
         * END: Test Scenario one
         * ***/

        // Current spat should only contain the ONLY one current movement event for each movement state.
        /****
         * START: Test Scenario two: There are two Yellow phases [1,5] in the current SPAT movement event
         * ***/
        // Add future movement events
        monitor_dpp_ptr->update_spat_future_movement_events(spat_msg_two_ptr, tsc_state_ptr);
        for (auto movement_state : spat_msg_two_ptr->get_intersection().states)
        {
            int sg = (int)movement_state.signal_group;
            SPDLOG_DEBUG("\n");
            if (sg == 1 || sg == 5)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 30 secs
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 10 secs
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 2 || sg == 6)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain: // Red
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // start with yellow (current yellow duration intial =0), and change red start time = yellow start time and end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 20 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance: // Yellow
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // last 0 secs as current yellow duration is initial 0.
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 0 secs as current yellow duration is initial 0.
                        }
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed: // Green
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 10 secs, but it starts with second desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 3)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red.
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 10 secs, but it starts with third desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 4 || sg == 8)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs, but it starts with fourth desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
        }
        /****
         * END: Test Scenario two
         * ***/

        // Current spat should only contain the ONLY one current movement event for each movement state.
        /****
         * START: Test Scenario three: They are all Red in the current SPAT movement events
         * ***/
        // Add future movement events
        monitor_dpp_ptr->update_spat_future_movement_events(spat_msg_three_ptr, tsc_state_ptr);
        for (auto movement_state : spat_msg_three_ptr->get_intersection().states)
        {
            int sg = (int)movement_state.signal_group;
            SPDLOG_DEBUG("\n");
            if (sg == 1 || sg == 5)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain://Red
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 30 secs
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance: //Yellow
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed://Green
                        ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // last 10 secs
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 2 || sg == 6)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain: // Red
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.min_end_time); // start with yellow (current yellow duration intial =0), and change red start time = yellow start time and end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 20 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance: // Yellow
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.min_end_time); // last 0 secs as current yellow duration is initial 0.
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 0 secs as current yellow duration is initial 0.
                        }
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed: // Green
                        ASSERT_EQ(current_hour_in_tenths_secs + 100, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // last 10 secs, but it starts with second desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 3)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red.
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 200, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // last 10 secs, but it starts with third desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
            else if (sg == 4 || sg == 8)
            {
                SPDLOG_DEBUG("current_hour_in_tenths_secs {0}", current_hour_in_tenths_secs);
                SPDLOG_DEBUG("signal group =  {0}", sg);
                SPDLOG_DEBUG("phase_state \t start_time \t min_end_time");
                for (auto movement_event : movement_state.state_time_speed)
                {
                    std::string state_name = "";
                    switch (movement_event.event_state)
                    {
                    case signal_phase_and_timing::movement_phase_state::stop_and_remain:
                        if (movement_event.timing.start_time == current_hour_in_tenths_secs)
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.min_end_time); // start with red, and change red end time to green start time
                        }
                        else
                        {
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                            ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs from the last red
                        }
                        state_name = "red";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_clearance:
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 0 secs as desired yellow duration and red clearance are initial 0.
                        state_name = "yellow";
                        break;
                    case signal_phase_and_timing::movement_phase_state::protected_movement_allowed:
                        ASSERT_EQ(current_hour_in_tenths_secs + 300, movement_event.timing.start_time);
                        ASSERT_EQ(current_hour_in_tenths_secs + 400, movement_event.timing.min_end_time); // last 10 secs, but it starts with fourth desired green
                        state_name = "green";
                        break;

                    default:
                        break;
                    }
                    SPDLOG_DEBUG("{0} \t\t {1} \t\t {2}", state_name, movement_event.timing.start_time, movement_event.timing.min_end_time);
                }
            }
        }
        /****
         * END: Test Scenario three
         * ***/
    }
}