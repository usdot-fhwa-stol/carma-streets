#include <gtest/gtest.h>
#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>

#include "ntcip_oids.h"
#include "monitor_tsc_state.h"
#include "streets_configuration.h"

using testing::_;

namespace traffic_signal_controller_service
{
    class mock_snmp_client : public snmp_client
    {
        
        public:
        mock_snmp_client(const std::string& ip, const int& port) : snmp_client(ip, port){}
    
        mock_snmp_client(mock_snmp_client& t, const std::string ip = "", const int port = 0): snmp_client(ip, port){}
        virtual ~mock_snmp_client(void){};
        MOCK_METHOD3(process_snmp_request, bool(const std::string& input_oid, const request_type& request_type,snmp_response_obj& val));
        
    };   


    TEST(traffic_signal_controller_service, test_get_tsc_state)
    {
        streets_service::streets_configuration::initialize_logger();
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        mock_snmp_client mock_client_worker(dummy_ip, dummy_port);

        auto unique_client = std::make_unique<mock_snmp_client>(mock_client_worker);

        int phase_num = 0;
        const std::string&input_oid = "";
        request_type request_type = request_type::GET;

        // Test get max channels
        snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.val_int = 4;
        max_channels_in_tsc.type = snmp_response_obj::response_type::INTEGER;
        ON_CALL(*unique_client, process_snmp_request(ntcip_oids::MAX_CHANNELS, _ , _) )
            .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(max_channels_in_tsc), testing::Return(true)));
        
        //get_vehicle_phase channel
        for(int i = 1; i <= max_channels_in_tsc.val_int; ++i){
            
            // Define Control Type
            snmp_response_obj channel_control_resp;
            channel_control_resp.val_int = 2;
            std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
            ON_CALL(*unique_client, process_snmp_request(channel_control_oid, _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(channel_control_resp), testing::Return(true)));

            // Define Control Source
            snmp_response_obj control_source_resp;
            control_source_resp.val_int = i;
            std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
            ON_CALL(*unique_client, process_snmp_request(control_source_oid, _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(control_source_resp), testing::Return(true)));

            // Define Sequence Data
            snmp_response_obj seq_data;
            seq_data.val_string = {char(1),char(2)};
            std::string seq_data_ring1_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(1);
            ON_CALL(*unique_client, process_snmp_request(seq_data_ring1_oid, _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(seq_data), testing::Return(true)));

            seq_data.val_string = {char(3),char(4)};
            std::string seq_data_ring2_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(2);
            ON_CALL(*unique_client, process_snmp_request(seq_data_ring2_oid, _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(seq_data), testing::Return(true)));

             // Define get min green
            std::string min_green_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(i);
            snmp_response_obj min_green;
            min_green.val_int = 20;
            ON_CALL(*unique_client, process_snmp_request(min_green_oid , _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(min_green), testing::Return(true)));
            

            // Define get max green
            std::string max_green_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(i);
            snmp_response_obj max_green;
            max_green.val_int = 30;
            ON_CALL(*unique_client, process_snmp_request(max_green_oid , _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(max_green), testing::Return(true)));


            // Define get yellow Duration
            std::string yellow_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(i);
            snmp_response_obj yellow_duration;
            yellow_duration.val_int = 40;
            ON_CALL(*unique_client, process_snmp_request(yellow_oid , _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(yellow_duration), testing::Return(true)));

            

            // Define red clearance
            std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(i);
            snmp_response_obj red_clearance_duration;
            red_clearance_duration.val_int = 10;
            ON_CALL(*unique_client, process_snmp_request(red_clearance_oid , _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(red_clearance_duration), testing::Return(true)));


            //Define get concurrent phases
            std::string concurrent_phase_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(i);
            snmp_response_obj concurrent_phase_resp;
            concurrent_phase_resp.val_string = {char(5), char(6)};
            ON_CALL(*unique_client, process_snmp_request(concurrent_phase_oid , _ , _) )
                .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(concurrent_phase_resp), testing::Return(true)));
        }
        
        std::shared_ptr<mock_snmp_client> shared_client = std::move(unique_client);
        traffic_signal_controller_service::tsc_state worker(shared_client);
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

        //Test tsc_config_state
        std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config_state = worker.get_tsc_config_state();
        EXPECT_EQ(tsc_config_state->tsc_config_list.front().signal_group_id, 2);
        EXPECT_EQ(tsc_config_state->tsc_config_list.front().red_clearance, 1000);

    }

    TEST(traffic_signal_controller_service, test_get_following_movement_events)
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

        streets_service::streets_configuration::initialize_logger();
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        mock_snmp_client mock_client_worker(dummy_ip, dummy_port);

        auto unique_client = std::make_unique<mock_snmp_client>(mock_client_worker);

        traffic_signal_controller_service::tsc_state worker(std::move(unique_client));
        traffic_signal_controller_service::signal_group_state phase_1_state;
        phase_1_state.signal_group_id = 1;
        phase_1_state.phase_num = 1;
        phase_1_state.min_green = 1000;
        phase_1_state.max_green = 2000;
        phase_1_state.green_duration = 1000;
        phase_1_state.yellow_duration = 1000;
        phase_1_state.red_clearance = 1000;
        phase_1_state.red_duration = 10000;
        
        worker.signal_group_state_map_.insert({1, phase_1_state});

        traffic_signal_controller_service::signal_group_state phase_2_state;
        phase_2_state = phase_1_state;
        phase_2_state.signal_group_id = 2;
        phase_2_state.phase_num = 2;

        worker.signal_group_state_map_.insert({2, phase_2_state});

        traffic_signal_controller_service::signal_group_state phase_3_state;
        phase_3_state = phase_1_state;
        phase_3_state.signal_group_id = 3;
        phase_3_state.phase_num = 3;

        spat_msg_ptr->intersections.push_back(intersection_state);
        worker.add_future_movement_events(spat_msg_ptr);
        
        // Check if movement events have been added
        EXPECT_TRUE(spat_msg_ptr->intersections.front().states.front().state_time_speed.size() > 1);
        for(auto it : spat_msg_ptr->intersections.front().states.front().state_time_speed){
            
            if(it.event_state == signal_phase_and_timing::movement_phase_state::protected_movement_allowed)
                EXPECT_EQ(it.timing.min_end_time - it.timing.start_time , phase_1_state.green_duration/100);
            
            else if(it.event_state == signal_phase_and_timing::movement_phase_state::protected_clearance)
                EXPECT_EQ(it.timing.min_end_time - it.timing.start_time, phase_1_state.yellow_duration/100);
            
            else if(it.event_state == signal_phase_and_timing::movement_phase_state::stop_and_remain)
                EXPECT_EQ(it.timing.min_end_time - it.timing.start_time, phase_1_state.red_duration/100);
            else 
                GTEST_FAIL();
        }       

        // Test exception 
        signal_phase_and_timing::intersection_state intersection_state_2;

        signal_phase_and_timing::movement_state state_4;
        state_4.state_time_speed.push_back(event_1);
        state_4.state_time_speed.push_back(event_2);
        intersection_state_2.states.push_back(state_4);
        
        auto spat_msg_ptr_2 = std::make_shared<signal_phase_and_timing::spat>();
        spat_msg_ptr->intersections.push_back(intersection_state_2);
        EXPECT_THROW(worker.add_future_movement_events(spat_msg_ptr),monitor_states_exception);

    }
}
