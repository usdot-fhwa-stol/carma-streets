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
            EXPECT_EQ(state.second.min_green, 20);
            EXPECT_EQ(state.second.max_green, 30);
            EXPECT_EQ(state.second.yellow_duration, 4);
            EXPECT_EQ(state.second.red_clearance,1);
        }

    }
}
