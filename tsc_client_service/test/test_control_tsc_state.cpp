#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>

#include "ntcip_oids.h"
#include "control_tsc_state.h"
#include "streets_configuration.h"
#include <mock_snmp_client.h>

using testing::_;
using testing::Return;
using testing::SetArgReferee;
using namespace std::chrono;

namespace traffic_signal_controller_service
{ 

    TEST(traffic_signal_controller_service, test_control_tsc_state)
    {
        // Initialize Clock singleton in realtime
        streets_service::streets_clock_singleton::create(false);
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;

        auto mock_client = std::make_shared<mock_snmp_client>();

        int phase_num = 0;
        const std::string&input_oid = "";
        auto request_type= streets_snmp_cmd::REQUEST_TYPE::GET;

        // Test get max channels
        streets_snmp_cmd::snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.val_int =8;
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
        seq_data.val_string = {char(1),char(2), char(3),char(4)};
        std::string seq_data_ring1_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(1);

        EXPECT_CALL(*mock_client, process_snmp_request(seq_data_ring1_oid, request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
            SetArgReferee<2>(seq_data), 
            Return(true)));

        seq_data.val_string = {char(5),char(6), char(7),char(8)};
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
                concurrent_phase_resp.val_string = {char(5), char(6)};
            }
            else if (i == 3|| i == 4) {
                concurrent_phase_resp.val_string = {char(7), char(8)};

            }
            else if (i == 5|| i == 6) {
                concurrent_phase_resp.val_string = {char(1), char(2)};

            }
            else if (i == 7|| i == 8) {
                concurrent_phase_resp.val_string = {char(3), char(4)};

            }
            EXPECT_CALL(*mock_client, process_snmp_request(concurrent_phase_oid , request_type , _) ).Times(1).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(concurrent_phase_resp), 
                testing::Return(true)));
        }

        // Define Control Type
        streets_snmp_cmd::snmp_response_obj hold_control;
        hold_control.val_int = 255;
        hold_control.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        
        EXPECT_CALL(*mock_client, process_snmp_request(_, streets_snmp_cmd::REQUEST_TYPE::SET , _) )
            .WillRepeatedly(testing::DoAll(testing::Return(true)));

        
        streets_desired_phase_plan::streets_desired_phase_plan desired_phase_plan;

        // Define Desired Phase plan
        system_clock clock;
        streets_desired_phase_plan::signal_group2green_phase_timing event1;
        event1.start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(1)).count();
        event1.end_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(5)).count();
        event1.signal_groups = {1,5};

        desired_phase_plan.desired_phase_plan.push_back(event1);

        streets_desired_phase_plan::signal_group2green_phase_timing event2;
        event2.start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(6)).count();;
        event2.end_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(10)).count();
        event2.signal_groups = {2,6};

        desired_phase_plan.desired_phase_plan.push_back(event2);
        auto desired_phase_plan_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>(desired_phase_plan);
        
        streets_desired_phase_plan::signal_group2green_phase_timing event3;
        event3.start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(11)).count();;
        event3.end_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(20)).count();
        event3.signal_groups = {7,8};
        desired_phase_plan.desired_phase_plan.push_back(event3);
        auto desired_phase_plan_ptr_2 = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>(desired_phase_plan);
        // Define Worker
        auto _tsc_state = std::make_shared<tsc_state>(mock_client);
        _tsc_state->initialize();
        traffic_signal_controller_service::control_tsc_state worker(mock_client, _tsc_state);
        
        
        // Test update queue
        std::queue<streets_snmp_cmd::snmp_cmd_struct> control_commands_queue;
        EXPECT_NO_THROW(worker.update_tsc_control_queue(desired_phase_plan_ptr,control_commands_queue));
        EXPECT_NO_THROW(worker.update_tsc_control_queue(desired_phase_plan_ptr_2,control_commands_queue));

        desired_phase_plan_ptr->desired_phase_plan.back().signal_groups = {1,6};
        EXPECT_THROW(worker.update_tsc_control_queue(desired_phase_plan_ptr,control_commands_queue), control_tsc_state_exception);

        // Test streets_snmp_cmd::snmp_cmd_struct
        streets_snmp_cmd::snmp_cmd_struct test_control_obj(event1.start_time,streets_snmp_cmd::PHASE_CONTROL_TYPE::HOLD_VEH_PHASES, 0);
        // EXPECT_TRUE(test_control_obj.run());

        streets_snmp_cmd::snmp_cmd_struct test_control_obj_2( event1.start_time,streets_snmp_cmd::PHASE_CONTROL_TYPE::OMIT_VEH_PHASES, 0);
        // EXPECT_TRUE(test_control_obj_2.run());

        // Test empty desired phase plan
        streets_desired_phase_plan::streets_desired_phase_plan desired_phase_plan_2;
        auto dpp_ptr_2 = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>(desired_phase_plan_2);
        EXPECT_NO_THROW(worker.update_tsc_control_queue(dpp_ptr_2,control_commands_queue));

        auto pcs_ptr = std::make_shared<streets_phase_control_schedule::streets_phase_control_schedule>();
        std::string input_schedule_str = "{\"MsgType\":\"Schedule\",\"Schedule\":[{\"commandEndTime\":0,\"commandPhase\":2,\"commandStartTime\":0,\"commandType\":\"hold\"},{\"commandEndTime\":24,\"commandPhase\":4,\"commandStartTime\":5,\"commandType\":\"hold\"},{\"commandEndTime\":44,\"commandPhase\":2,\"commandStartTime\":29,\"commandType\":\"hold\"},{\"commandEndTime\":64,\"commandPhase\":4,\"commandStartTime\":49,\"commandType\":\"hold\"},{\"commandEndTime\":12,\"commandPhase\":2,\"commandStartTime\":11,\"commandType\":\"forceoff\"},{\"commandEndTime\":69,\"commandPhase\":4,\"commandStartTime\":68,\"commandType\":\"forceoff\"},{\"commandEndTime\":109,\"commandPhase\":2,\"commandStartTime\":108,\"commandType\":\"forceoff\"},{\"commandEndTime\":129,\"commandPhase\":4,\"commandStartTime\":128,\"commandType\":\"forceoff\"},{\"commandEndTime\":23,\"commandPhase\":4,\"commandStartTime\":0,\"commandType\":\"call_veh\"},{\"commandEndTime\":133,\"commandPhase\":1,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":133,\"commandPhase\":3,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":0,\"commandPhase\":6,\"commandStartTime\":0,\"commandType\":\"hold\"},{\"commandEndTime\":24,\"commandPhase\":7,\"commandStartTime\":5,\"commandType\":\"hold\"},{\"commandEndTime\":44,\"commandPhase\":6,\"commandStartTime\":29,\"commandType\":\"hold\"},{\"commandEndTime\":64,\"commandPhase\":7,\"commandStartTime\":49,\"commandType\":\"hold\"},{\"commandEndTime\":12,\"commandPhase\":6,\"commandStartTime\":11,\"commandType\":\"forceoff\"},{\"commandEndTime\":69,\"commandPhase\":7,\"commandStartTime\":68,\"commandType\":\"forceoff\"},{\"commandEndTime\":109,\"commandPhase\":6,\"commandStartTime\":108,\"commandType\":\"forceoff\"},{\"commandEndTime\":129,\"commandPhase\":7,\"commandStartTime\":128,\"commandType\":\"forceoff\"},{\"commandEndTime\":23,\"commandPhase\":7,\"commandStartTime\":0,\"commandType\":\"call_veh\"},{\"commandEndTime\":133,\"commandPhase\":5,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":133,\"commandPhase\":8,\"commandStartTime\":0,\"commandType\":\"omit_veh\"}]}";
        pcs_ptr->fromJson(input_schedule_str);
        worker.update_tsc_control_queue(pcs_ptr, control_commands_queue);
    }
    

    TEST(traffic_signal_controller_service, update_tsc_control_queue)
    {
        // Define Worker
        auto mock_client = std::make_shared<mock_snmp_client>();
        auto _tsc_state = std::make_shared<tsc_state>(mock_client);
        _tsc_state->initialize();
        traffic_signal_controller_service::control_tsc_state worker(mock_client, _tsc_state);

        //Update queue with new schedule that has commands
        auto pcs_ptr = std::make_shared<streets_phase_control_schedule::streets_phase_control_schedule>();
        std::string input_schedule_str = "{\"MsgType\":\"Schedule\",\"Schedule\":[{\"commandEndTime\":0,\"commandPhase\":2,\"commandStartTime\":0,\"commandType\":\"hold\"},{\"commandEndTime\":24,\"commandPhase\":4,\"commandStartTime\":5,\"commandType\":\"hold\"},{\"commandEndTime\":44,\"commandPhase\":2,\"commandStartTime\":29,\"commandType\":\"hold\"},{\"commandEndTime\":64,\"commandPhase\":4,\"commandStartTime\":49,\"commandType\":\"hold\"},{\"commandEndTime\":12,\"commandPhase\":2,\"commandStartTime\":11,\"commandType\":\"forceoff\"},{\"commandEndTime\":69,\"commandPhase\":4,\"commandStartTime\":68,\"commandType\":\"forceoff\"},{\"commandEndTime\":109,\"commandPhase\":2,\"commandStartTime\":108,\"commandType\":\"forceoff\"},{\"commandEndTime\":129,\"commandPhase\":4,\"commandStartTime\":128,\"commandType\":\"forceoff\"},{\"commandEndTime\":23,\"commandPhase\":4,\"commandStartTime\":0,\"commandType\":\"call_veh\"},{\"commandEndTime\":133,\"commandPhase\":1,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":133,\"commandPhase\":3,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":0,\"commandPhase\":6,\"commandStartTime\":0,\"commandType\":\"hold\"},{\"commandEndTime\":24,\"commandPhase\":7,\"commandStartTime\":5,\"commandType\":\"hold\"},{\"commandEndTime\":44,\"commandPhase\":6,\"commandStartTime\":29,\"commandType\":\"hold\"},{\"commandEndTime\":64,\"commandPhase\":7,\"commandStartTime\":49,\"commandType\":\"hold\"},{\"commandEndTime\":12,\"commandPhase\":6,\"commandStartTime\":11,\"commandType\":\"forceoff\"},{\"commandEndTime\":69,\"commandPhase\":7,\"commandStartTime\":68,\"commandType\":\"forceoff\"},{\"commandEndTime\":109,\"commandPhase\":6,\"commandStartTime\":108,\"commandType\":\"forceoff\"},{\"commandEndTime\":129,\"commandPhase\":7,\"commandStartTime\":128,\"commandType\":\"forceoff\"},{\"commandEndTime\":23,\"commandPhase\":7,\"commandStartTime\":0,\"commandType\":\"call_veh\"},{\"commandEndTime\":133,\"commandPhase\":5,\"commandStartTime\":0,\"commandType\":\"omit_veh\"},{\"commandEndTime\":133,\"commandPhase\":8,\"commandStartTime\":0,\"commandType\":\"omit_veh\"}]}";
        pcs_ptr->fromJson(input_schedule_str);
        
        // Test update queue with clear schedule
        std::queue<streets_snmp_cmd::snmp_cmd_struct> control_commands_queue;
        worker.update_tsc_control_queue(pcs_ptr, control_commands_queue);
        input_schedule_str = "{\"MsgType\":\"Schedule\",\"Schedule\":\"Clear\"}";
        pcs_ptr->fromJson(input_schedule_str);
        worker.update_tsc_control_queue(pcs_ptr, control_commands_queue);
        std::shared_ptr<streets_phase_control_schedule::streets_phase_control_schedule> pcs_ptr_null;
        worker.update_tsc_control_queue(pcs_ptr_null, control_commands_queue);
    }
}