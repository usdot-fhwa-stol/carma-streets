#include <gtest/gtest.h>
#include <iostream>
#include "monitor_tsc_state.h"
#include <gmock/gmock.h>
#include "ntcip_oids.h"
#include <gtest/gtest_prod.h>

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

    TEST(test_monitor_tsc_state, test_signal_group_phase_mapping)
    {
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        snmp_client client_worker(dummy_ip, dummy_port);

        tsc_state worker(std::make_shared<snmp_client> (client_worker));
        // Get Max channels
        EXPECT_EQ(worker.get_max_channels(), 0);
        // Get vehicle phase channels - using arbitrary max channels
        int maximum_channels = 6;
        EXPECT_TRUE(worker.get_vehicle_phase_channels(maximum_channels).empty());

        std::vector<int> phase_channels = {1,2,3,4};
        worker.map_phase_and_signalgroup(phase_channels);

    }

    TEST(traffic_signal_controller_service, test_get_tsc_state)
    {
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        mock_snmp_client mock_client_worker(dummy_ip, dummy_port);
        
        auto shared_client = std::make_shared<mock_snmp_client> (mock_client_worker);
        traffic_signal_controller_service::tsc_state worker(shared_client);

        int phase_num = 0;
        const std::string&input_oid = "";;
        request_type request_type = request_type::GET;

        // Test get max channels
        snmp_response_obj max_channels_in_tsc;
        max_channels_in_tsc.val_int = 10;
        max_channels_in_tsc.type = snmp_response_obj::response_type::INTEGER;
        ON_CALL(*shared_client, process_snmp_request(ntcip_oids::MAX_CHANNELS, _ , _) )
            .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(max_channels_in_tsc), testing::Return(true)));
        EXPECT_EQ(worker.get_max_channels(), 10);
        
        // Test get min green
        std::string min_green_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(phase_num);
        snmp_response_obj min_green;
        min_green.val_int = 20;
        ON_CALL(*shared_client, process_snmp_request(min_green_oid , _ , _) )
            .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(min_green), testing::Return(true)));
        EXPECT_EQ(worker.get_min_green(phase_num), 20);

        // Test get max green
        std::string max_green_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(phase_num);
        snmp_response_obj max_green;
        max_green.val_int = 30;
        ON_CALL(*shared_client, process_snmp_request(max_green_oid , _ , _) )
            .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(max_green), testing::Return(true)));

        EXPECT_EQ(worker.get_max_green(phase_num), 30);

        // Test get yellow Duration
        std::string yellow_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(phase_num);
        snmp_response_obj yellow_duration;
        yellow_duration.val_int = 40;
        ON_CALL(*shared_client, process_snmp_request(yellow_oid , _ , _) )
            .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(yellow_duration), testing::Return(true)));

        EXPECT_EQ(worker.get_yellow_duration(phase_num), 4);

        // Test red clearance
        std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(phase_num);
        snmp_response_obj red_clearance_duration;
        red_clearance_duration.val_int = 10;
        ON_CALL(*shared_client, process_snmp_request(red_clearance_oid , _ , _) )
            .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(red_clearance_duration), testing::Return(true)));

        EXPECT_EQ(worker.get_red_clearance(phase_num), 1);

        // Test Red Duration
        EXPECT_EQ(worker.get_red_duration(phase_num), 0);

        // Test get following phases
        worker.phase_seq_ring1_ = {1,2,3,4};
        EXPECT_TRUE(!worker.get_following_phases(1).empty());
        worker.phase_seq_ring2_ = {5,6,7,8};
        EXPECT_TRUE(!worker.get_following_phases(6).empty());

        //Test get Phase sequence for ring
        std::string phase_seq_oid_ring = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(phase_num);
        snmp_response_obj phase_seq_resp;
        phase_seq_resp.val_string = {'2', '4'};
        ON_CALL(*shared_client, process_snmp_request(phase_seq_oid_ring , _ , _) )
            .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(phase_seq_resp), testing::Return(true)));
        EXPECT_TRUE(worker.phase_seq(phase_num).empty());

        //Test get concurrent phases
        std::string concurrent_phase_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(phase_num);
        snmp_response_obj concurrent_phase_resp;
        concurrent_phase_resp.val_string = {'5', '6'};
        ON_CALL(*shared_client, process_snmp_request(concurrent_phase_oid , _ , _) )
            .WillByDefault(testing::DoAll(testing::SetArgReferee<2>(concurrent_phase_resp), testing::Return(true)));

        EXPECT_FALSE(worker.get_concurrent_phases(phase_num).empty());

    }
}
