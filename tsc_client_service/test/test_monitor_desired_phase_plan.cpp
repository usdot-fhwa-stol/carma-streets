#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>
#include "monitor_desired_phase_plan.h"
#include "monitor_desired_phase_plan_exception.h"
#include "mock_snmp_client.h"
#include <spdlog/spdlog.h>
#include <chrono>

using testing::_;
using testing::Return;
using testing::SetArgReferee;
namespace traffic_signal_controller_service
{

    class test_monitor_desired_phase_plan : public ::testing::Test
    {
    public:
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_two_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_msg_three_ptr;
        std::shared_ptr<monitor_desired_phase_plan> monitor_dpp_ptr;
        std::shared_ptr<mock_snmp_client> mock_snmp;
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

            // Create 3 test spat messages.
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
            mock_snmp = std::make_shared<mock_snmp_client>();
            tsc_state_ptr = std::make_shared<tsc_state>(mock_snmp);
            monitor_dpp_ptr = std::make_shared<monitor_desired_phase_plan>(mock_snmp);

            SPDLOG_INFO("Setup complete");
        }
    };

    TEST_F(test_monitor_desired_phase_plan, update_desired_phase_plan)
    {
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
        SPDLOG_INFO("Test case update spat future movement events!");
        // gmock SNMP response ---------------------------------------------------------------------------------------------------------------------
        // Test get max channels
        snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.val_int = 16;
        max_channels_in_tsc.type = snmp_response_obj::response_type::INTEGER;

        request_type request_type = request_type::GET;

        EXPECT_CALL( *mock_snmp, process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(max_channels_in_tsc), 
            Return(true)));

        snmp_response_obj max_rings_in_tsc;
        max_rings_in_tsc.val_int = 4;
        max_rings_in_tsc.type = snmp_response_obj::response_type::INTEGER;
        EXPECT_CALL( *mock_snmp, process_snmp_request(ntcip_oids::MAX_RINGS, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(max_rings_in_tsc), 
            Return(true)));
        // Define Sequence Data
        snmp_response_obj seq_data;
        seq_data.val_string = {char(1),char(2), char(3), char(4)};
        std::string seq_data_ring1_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(1);

        EXPECT_CALL(*mock_snmp, process_snmp_request(seq_data_ring1_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        seq_data.val_string = {char(5),char(6), char(7), char(8)};
        std::string seq_data_ring2_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(2);
        EXPECT_CALL(*mock_snmp, process_snmp_request(seq_data_ring2_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        seq_data.val_string = {};
        std::string seq_data_ring3_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(3);
        EXPECT_CALL(*mock_snmp, process_snmp_request(seq_data_ring3_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        std::string seq_data_ring4_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(4);
        EXPECT_CALL(*mock_snmp, process_snmp_request(seq_data_ring4_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));
        //get_vehicle_phase channel
        for(int i = 1; i <= max_channels_in_tsc.val_int; ++i){
            // phase number
            // ring 1 ->  | 1 | 2 || 3 | 4 ||
            // ring 2 ->  | 5 | 6 || 7 | 8 ||
            // signal group
            // ring 1 ->  | 3  | 7 || 9  | 14 ||
            // ring 2 ->  | 11 | 6 || 15 | 1  ||
            //
            switch(i) {
                case 1: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 1;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 2: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 2;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 3: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 3;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 4: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 4;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 5: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 5;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 6: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 6;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 7: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 7;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 8: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 8;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true))); 
                    break;
                }
                default: {
                    snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 0;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    // Note this OID is not actually called for any non vehicle/pedestrian phases which is why the Times() assertion 
                    // is not included
                    snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 0;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                }
            }
            
        }
        for(int i = 1; i <= 8; i++) {
            

             // Define get min green
            std::string min_green_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(i);
            snmp_response_obj min_green;
            min_green.val_int = 100;
            EXPECT_CALL(*mock_snmp, process_snmp_request(min_green_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(min_green), 
                Return(true)));
            

            // Define get max green
            std::string max_green_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(i);
            snmp_response_obj max_green;
            max_green.val_int = 300;
            EXPECT_CALL(*mock_snmp, process_snmp_request(max_green_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(max_green), 
                Return(true)));


            // Define get yellow Duration
            std::string yellow_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(i);
            snmp_response_obj yellow_duration;
            yellow_duration.val_int = 0;
            EXPECT_CALL(*mock_snmp, process_snmp_request(yellow_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(yellow_duration), 
                Return(true)));

            

            // Define red clearance
            std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(i);
            snmp_response_obj red_clearance_duration;
            red_clearance_duration.val_int = 0;
            EXPECT_CALL(*mock_snmp, process_snmp_request(red_clearance_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(red_clearance_duration), 
                Return(true)));


            //Define get concurrent phases
            std::string concurrent_phase_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(i);
            snmp_response_obj concurrent_phase_resp;
            if ( i == 1 || i == 2) {
                concurrent_phase_resp.val_string = {char(5), char(6)};
            }
            else if (i == 3|| i == 4) {
                concurrent_phase_resp.val_string = {char(7), char(8)};
            }
            else if (i == 5|| i == 6) {
                concurrent_phase_resp.val_string = {char(1), char(2)};
            }
            // if 7 || 8
            else {
               concurrent_phase_resp.val_string = {char(3), char(4)}; 
            }
            EXPECT_CALL(*mock_snmp, process_snmp_request(concurrent_phase_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(concurrent_phase_resp), 
                Return(true)));
        }
        // gmock SNMP response END ----------------------------------------------------------------------------------------------------------------------------------

        tsc_state_ptr->initialize();

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
        SPDLOG_INFO("Do we make it here.");

        // Add future movement events with an EMPTY desired phase plan, and it should not modify the SPAT message
        // try {
            monitor_dpp_ptr->update_spat_future_movement_events(spat_msg_ptr, tsc_state_ptr);
            SPDLOG_INFO("Is it failing here");
        // }
        // catch (const monitor_desired_phase_plan_exception &e) {
        //     ASSERT_STREQ(e.what(), "Desired phase plan is empty. No update.");
        // }
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