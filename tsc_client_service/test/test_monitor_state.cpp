#include <gtest/gtest.h>
#include <iostream>
#include <gmock/gmock.h>

#include "ntcip_oids.h"
#include "monitor_tsc_state.h"
#include "streets_configuration.h"
#include "mock_snmp_client.h"

using testing::_;
using testing::Return;
using testing::SetArgReferee;


namespace traffic_signal_controller_service
{
    TEST(test_monitor_state, poll_vehicle_pedestrian_calls){
        auto mock_client = std::make_shared<mock_snmp_client>();
        const std::string&input_oid = "";
        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;

        // Test get max channels
        streets_snmp_cmd::snmp_response_obj vehicle_calls;
        vehicle_calls.val_int = 4;
        vehicle_calls.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        EXPECT_CALL( *mock_client, process_snmp_request(ntcip_oids::PHASE_STATUS_GROUP_VEH_CALLS + ".1", request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(vehicle_calls), 
            Return(true)));
        vehicle_calls.val_int = 0;

         EXPECT_CALL( *mock_client, process_snmp_request(ntcip_oids::PHASE_STATUS_GROUP_VEH_CALLS + ".2", request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(vehicle_calls), 
            Return(true)));
        vehicle_calls.val_int = 4;

        EXPECT_CALL( *mock_client, process_snmp_request(ntcip_oids::PHASE_STATUS_GROUP_PED_CALLS + ".1", request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(vehicle_calls), 
            Return(true)));
        vehicle_calls.val_int = 0;

         EXPECT_CALL( *mock_client, process_snmp_request(ntcip_oids::PHASE_STATUS_GROUP_PED_CALLS + ".2", request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(vehicle_calls), 
            Return(true)));

        // Test get max channels
        streets_snmp_cmd::snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.val_int = 4;
        max_channels_in_tsc.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        EXPECT_CALL( *mock_client, process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(max_channels_in_tsc), 
            Return(true)));

        streets_snmp_cmd::snmp_response_obj max_rings_in_tsc;
        max_rings_in_tsc.val_int = 4;
        max_rings_in_tsc.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        EXPECT_CALL( *mock_client, process_snmp_request(ntcip_oids::MAX_RINGS, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(max_rings_in_tsc), 
            Return(true)));
        // Define Sequence Data
        streets_snmp_cmd::snmp_response_obj seq_data;
        seq_data.val_string = {char(1),char(2)};
        std::string seq_data_ring1_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(1);

        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring1_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        seq_data.val_string = {char(3),char(4)};
        std::string seq_data_ring2_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(2);
        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring2_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        seq_data.val_string = {};
        std::string seq_data_ring3_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(3);
        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring3_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        std::string seq_data_ring4_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(4);
        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring4_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));
        
        //get_vehicle_phase channel
        for(int i = 1; i <= max_channels_in_tsc.val_int; ++i){
            
            // Define Control Type
            streets_snmp_cmd::snmp_response_obj channel_control_resp;
            channel_control_resp.val_int = 2;
            std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
            EXPECT_CALL(*mock_client, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(channel_control_resp), 
                testing::Return(true)));

            // Define Control Source
            streets_snmp_cmd::snmp_response_obj control_source_resp;
            control_source_resp.val_int = i;
            std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
            EXPECT_CALL(*mock_client, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(control_source_resp), 
                Return(true)));

            

             // Define get min green
            std::string min_green_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj min_green;
            min_green.val_int = 20;
            EXPECT_CALL(*mock_client, process_snmp_request(min_green_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(min_green), 
                Return(true)));
            

            // Define get max green
            std::string max_green_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj max_green;
            max_green.val_int = 30;
            EXPECT_CALL(*mock_client, process_snmp_request(max_green_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(max_green), 
                Return(true)));


            // Define get yellow Duration
            std::string yellow_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj yellow_duration;
            yellow_duration.val_int = 40;
            EXPECT_CALL(*mock_client, process_snmp_request(yellow_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(yellow_duration), 
                Return(true)));

            

            // Define red clearance
            std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj red_clearance_duration;
            red_clearance_duration.val_int = 10;
            EXPECT_CALL(*mock_client, process_snmp_request(red_clearance_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(red_clearance_duration), 
                Return(true)));

            //Define get concurrent phases
            std::string concurrent_phase_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj concurrent_phase_resp;
            if ( i == 1 || i == 2) {
                concurrent_phase_resp.val_string = {char(1), char(2)};
            }
            else if (i == 3|| i == 4) {
                concurrent_phase_resp.val_string = {char(3), char(4)};

            }
            EXPECT_CALL(*mock_client, process_snmp_request(concurrent_phase_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(concurrent_phase_resp), 
                testing::Return(true)));
        }
        traffic_signal_controller_service::tsc_state worker(mock_client);
        worker.initialize();
        worker.poll_vehicle_pedestrian_calls();
    }
   
    TEST(test_monitor_state, test_get_tsc_state)
    {
        auto mock_client = std::make_shared<mock_snmp_client>();

        int phase_num = 0;
        const std::string&input_oid = "";
        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;

        // Test get max channels
        streets_snmp_cmd::snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.val_int = 4;
        max_channels_in_tsc.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        EXPECT_CALL( *mock_client, process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(max_channels_in_tsc), 
            Return(true)));

        streets_snmp_cmd::snmp_response_obj max_rings_in_tsc;
        max_rings_in_tsc.val_int = 4;
        max_rings_in_tsc.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        EXPECT_CALL( *mock_client, process_snmp_request(ntcip_oids::MAX_RINGS, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(max_rings_in_tsc), 
            Return(true)));
        // Define Sequence Data
        streets_snmp_cmd::snmp_response_obj seq_data;
        seq_data.val_string = {char(1),char(2)};
        std::string seq_data_ring1_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(1);

        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring1_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        seq_data.val_string = {char(3),char(4)};
        std::string seq_data_ring2_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(2);
        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring2_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        seq_data.val_string = {};
        std::string seq_data_ring3_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(3);
        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring3_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        std::string seq_data_ring4_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(4);
        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring4_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));
        
        //get_vehicle_phase channel
        for(int i = 1; i <= max_channels_in_tsc.val_int; ++i){
            
            // Define Control Type
            streets_snmp_cmd::snmp_response_obj channel_control_resp;
            channel_control_resp.val_int = 2;
            std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
            EXPECT_CALL(*mock_client, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(channel_control_resp), 
                testing::Return(true)));

            // Define Control Source
            streets_snmp_cmd::snmp_response_obj control_source_resp;
            control_source_resp.val_int = i;
            std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
            EXPECT_CALL(*mock_client, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(control_source_resp), 
                Return(true)));

            

             // Define get min green
            std::string min_green_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj min_green;
            min_green.val_int = 20;
            EXPECT_CALL(*mock_client, process_snmp_request(min_green_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(min_green), 
                Return(true)));
            

            // Define get max green
            std::string max_green_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj max_green;
            max_green.val_int = 30;
            EXPECT_CALL(*mock_client, process_snmp_request(max_green_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(max_green), 
                Return(true)));


            // Define get yellow Duration
            std::string yellow_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj yellow_duration;
            yellow_duration.val_int = 40;
            EXPECT_CALL(*mock_client, process_snmp_request(yellow_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(yellow_duration), 
                Return(true)));

            

            // Define red clearance
            std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj red_clearance_duration;
            red_clearance_duration.val_int = 10;
            EXPECT_CALL(*mock_client, process_snmp_request(red_clearance_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(red_clearance_duration), 
                Return(true)));

            //Define get concurrent phases
            std::string concurrent_phase_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj concurrent_phase_resp;
            if ( i == 1 || i == 2) {
                concurrent_phase_resp.val_string = {char(1), char(2)};
            }
            else if (i == 3|| i == 4) {
                concurrent_phase_resp.val_string = {char(3), char(4)};

            }
            EXPECT_CALL(*mock_client, process_snmp_request(concurrent_phase_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(concurrent_phase_resp), 
                testing::Return(true)));
        }
        
        traffic_signal_controller_service::tsc_state worker(mock_client);
        worker.initialize();

        std::unordered_map<int, signal_group_state> state_map = worker.get_signal_group_state_map();

        for(auto const& state : state_map)
        {
            EXPECT_TRUE(state.second.phase_num <= 4); //Test defined for phases 1,2,3,4
            EXPECT_EQ(state.second.min_green, 20000);
            EXPECT_EQ(state.second.max_green, 30000);
            EXPECT_EQ(state.second.yellow_duration, 4000);
            EXPECT_EQ(state.second.red_clearance,1000);
        }

        std::unordered_map<int,int> ped_phase_map = worker.get_ped_phase_map();
        EXPECT_TRUE(ped_phase_map.empty());
        std::unordered_map<int,int> vehicle_phase_map = worker.get_vehicle_phase_map();
        EXPECT_FALSE(vehicle_phase_map.empty());
        std::unordered_map<int,int> signal_group_map = worker.get_signal_group_map();
        EXPECT_FALSE(signal_group_map.empty());

        //Test tsc_config_state
        std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config_state = worker.get_tsc_config_state();
        for (auto const& conf : tsc_config_state->tsc_config_list ) {
            if ( conf.signal_group_id == 1 || conf.signal_group_id == 2) {
                EXPECT_EQ( conf.concurrent_signal_groups.front(), 1);
                EXPECT_EQ( conf.concurrent_signal_groups.back(), 2);
            } else {
                EXPECT_EQ( conf.concurrent_signal_groups.front(), 3);
                EXPECT_EQ( conf.concurrent_signal_groups.back(), 4);
            }
            EXPECT_EQ( conf.red_clearance, 1000);
            EXPECT_EQ( conf.yellow_change_duration, 4000);
        }

    }

    TEST(test_monitor_state, test_get_following_movement_events)
    {
        // Create mock spat
        auto spat_msg_ptr = std::make_shared<signal_phase_and_timing::spat>();

        signal_phase_and_timing::intersection_state intersection_state;

        signal_phase_and_timing::movement_state state_1;
        state_1.signal_group = 1;
        signal_phase_and_timing::movement_event event_1;
        event_1.event_state = signal_phase_and_timing::movement_phase_state::stop_and_remain; //Red
        event_1.timing.start_time = 1000;
        event_1.timing.min_end_time = 1100;
        state_1.state_time_speed.push_back(event_1);
        intersection_state.states.push_back(state_1);

        signal_phase_and_timing::movement_state state_2;
        state_2.signal_group = 2;
        signal_phase_and_timing::movement_event event_2;
        event_2.event_state = signal_phase_and_timing::movement_phase_state::protected_clearance; //Yellow
        event_2.timing.start_time = 20;
        state_2.state_time_speed.push_back(event_2);

        intersection_state.states.push_back(state_2);

        signal_phase_and_timing::movement_state state_3;
        state_3.signal_group = 3;
        signal_phase_and_timing::movement_event event_3;
        event_3.event_state = signal_phase_and_timing::movement_phase_state::protected_movement_allowed; //Green
        event_3.timing.start_time = 30;
        state_3.state_time_speed.push_back(event_3);

        intersection_state.states.push_back(state_3);
        spat_msg_ptr->set_intersection(intersection_state);
        SPDLOG_INFO("Setting Intersection state!");

        auto mock_client = std::make_shared<mock_snmp_client>();

        traffic_signal_controller_service::tsc_state worker(mock_client);
        traffic_signal_controller_service::signal_group_state phase_1_state;
        phase_1_state.signal_group_id = 1;
        phase_1_state.phase_num = 1;
        phase_1_state.min_green = 1000;
        phase_1_state.max_green = 2000;
        phase_1_state.green_duration = 1000;
        phase_1_state.yellow_duration = 1000;
        phase_1_state.red_clearance = 1000;
        phase_1_state.red_duration = 10000;
        
        worker.signal_group_2tsc_state_map_.insert({1, phase_1_state});

        traffic_signal_controller_service::signal_group_state phase_2_state;
        phase_2_state = phase_1_state;
        phase_2_state.signal_group_id = 2;
        phase_2_state.phase_num = 2;

        worker.signal_group_2tsc_state_map_.insert({2, phase_2_state});

        traffic_signal_controller_service::signal_group_state phase_3_state;
        phase_3_state = phase_1_state;
        phase_3_state.signal_group_id = 3;
        phase_3_state.phase_num = 3;

        worker.add_future_movement_events(spat_msg_ptr);
        SPDLOG_INFO( "Updating future movement events in SPAT");
        auto new_intersection_state = spat_msg_ptr->get_intersection();
        
        // Check if movement events have been added
        EXPECT_TRUE(new_intersection_state.states.front().state_time_speed.size() > 1);
        for(auto it : new_intersection_state.states.front().state_time_speed){
            
            if(it.event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed)
                EXPECT_EQ(it.timing.min_end_time - it.timing.start_time , phase_1_state.green_duration/100);
            
            else if(it.event_state == signal_phase_and_timing::movement_phase_state::protected_clearance)
                EXPECT_EQ(it.timing.min_end_time - it.timing.start_time, phase_1_state.yellow_duration/100);
            
            else if(it.event_state == signal_phase_and_timing::movement_phase_state::stop_and_remain)
                EXPECT_EQ(it.timing.min_end_time - it.timing.start_time, phase_1_state.red_duration/100);
            else {
                GTEST_FATAL_FAILURE_("Invalid Event State!");
            }
        }       

        // Test exception 
        signal_phase_and_timing::intersection_state intersection_state_2;

        signal_phase_and_timing::movement_state state_4;
        state_4.state_time_speed.push_back(event_1);
        state_4.state_time_speed.push_back(event_2);
        intersection_state_2.states.push_back(state_4);
        
        auto spat_msg_ptr_2 = std::make_shared<signal_phase_and_timing::spat>();
        spat_msg_ptr->set_intersection(intersection_state_2);
        EXPECT_THROW(worker.add_future_movement_events(spat_msg_ptr),monitor_states_exception);

    }
    
    TEST(test_monitor_state, test_get_phase_number) {

        std::shared_ptr<mock_snmp_client> mock_snmp =  std::make_shared<mock_snmp_client>();
        tsc_state state(mock_snmp);
         // When phase to signal group map is empty, 
        ASSERT_THROW(state.get_phase_number(1), monitor_states_exception);
        // Test get max channels
        streets_snmp_cmd::snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.val_int = 16;
        max_channels_in_tsc.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;

        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;

        EXPECT_CALL( *mock_snmp, process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(max_channels_in_tsc), 
            Return(true)));

        streets_snmp_cmd::snmp_response_obj max_rings_in_tsc;
        max_rings_in_tsc.val_int = 4;
        max_rings_in_tsc.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        EXPECT_CALL( *mock_snmp, process_snmp_request(ntcip_oids::MAX_RINGS, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(max_rings_in_tsc), 
            Return(true)));
        // Define Sequence Data
        streets_snmp_cmd::snmp_response_obj seq_data;
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
                case 3: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 1;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 7: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 2;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 9: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 3;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 14: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 4;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 11: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 5;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 6: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 6;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 15: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 7;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true)));
                    break;
                }
                case 1: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 2;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
                    control_source_resp.val_int = 8;
                    std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(control_source_resp), 
                        Return(true))); 
                    break;
                }
                default: {
                    streets_snmp_cmd::snmp_response_obj channel_control_resp;
                    channel_control_resp.val_int = 0;
                    std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                    EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                        SetArgReferee<2>(channel_control_resp),
                        Return(true)));

                    // Define Control Source
                    // Note this OID is not actually called for any non vehicle/pedestrian phases which is why the Times() assertion 
                    // is not included
                    streets_snmp_cmd::snmp_response_obj control_source_resp;
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
            streets_snmp_cmd::snmp_response_obj min_green;
            min_green.val_int = 20;
            EXPECT_CALL(*mock_snmp, process_snmp_request(min_green_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(min_green), 
                Return(true)));
            

            // Define get max green
            std::string max_green_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj max_green;
            max_green.val_int = 30;
            EXPECT_CALL(*mock_snmp, process_snmp_request(max_green_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(max_green), 
                Return(true)));


            // Define get yellow Duration
            std::string yellow_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj yellow_duration;
            yellow_duration.val_int = 40;
            EXPECT_CALL(*mock_snmp, process_snmp_request(yellow_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(yellow_duration), 
                Return(true)));

            

            // Define red clearance
            std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj red_clearance_duration;
            red_clearance_duration.val_int = 10;
            EXPECT_CALL(*mock_snmp, process_snmp_request(red_clearance_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(red_clearance_duration), 
                Return(true)));


            //Define get concurrent phases
            std::string concurrent_phase_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(i);
            streets_snmp_cmd::snmp_response_obj concurrent_phase_resp;
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

        state.initialize();
        // phase number
        // ring 1 ->  | 1 | 2 || 3 | 4 ||
        // ring 2 ->  | 5 | 6 || 7 | 8 ||
        // signal group
        // ring 1 ->  | 3  | 7 || 9  | 14 ||
        // ring 2 ->  | 11 | 6 || 15 | 1  ||
        //
        ASSERT_EQ(1, state.get_phase_number(3));
        ASSERT_EQ(2, state.get_phase_number(7));
        ASSERT_EQ(3, state.get_phase_number(9));
        ASSERT_EQ(4, state.get_phase_number(14));
        ASSERT_EQ(5, state.get_phase_number(11));
        ASSERT_EQ(6, state.get_phase_number(6));
        ASSERT_EQ(7, state.get_phase_number(15));
        ASSERT_EQ(8, state.get_phase_number(1));
        ASSERT_THROW(state.get_phase_number(2), monitor_states_exception);
        ASSERT_THROW(state.get_phase_number(0), monitor_states_exception);


        ASSERT_EQ(3, state.get_vehicle_signal_group_id(1));
        ASSERT_EQ(7, state.get_vehicle_signal_group_id(2));
        ASSERT_EQ(9, state.get_vehicle_signal_group_id(3));
        ASSERT_EQ(14, state.get_vehicle_signal_group_id(4));
        ASSERT_EQ(11, state.get_vehicle_signal_group_id(5));
        ASSERT_EQ(6, state.get_vehicle_signal_group_id(6));
        ASSERT_EQ(15, state.get_vehicle_signal_group_id(7));
        ASSERT_EQ(1, state.get_vehicle_signal_group_id(8));
        ASSERT_THROW(state.get_vehicle_signal_group_id(9), monitor_states_exception);
        ASSERT_THROW(state.get_vehicle_signal_group_id(0), monitor_states_exception);


        

       
    }
}
