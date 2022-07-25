#include <gtest/gtest.h>
#include <iostream>
#include "monitor_tsc_state.h"
#include <gmock/gmock.h>
#include "ntcip_oids.h"

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
        testing::NiceMock<mock_snmp_client> mock_client_worker(dummy_ip, dummy_port);
        
        auto shared_client = std::make_shared<mock_snmp_client> (mock_client_worker);
        traffic_signal_controller_service::tsc_state worker(shared_client);


        const std::string&input_oid = "";
        request_type req = request_type::GET;
        const request_type& request_type = req;
        snmp_response_obj val;
        ON_CALL(mock_client_worker, process_snmp_request(input_oid, request_type, val)).WillByDefault(testing::Return(true));

        // Test get max channels
        EXPECT_EQ(worker.get_max_channels(), 0);

        EXPECT_EQ(worker.get_min_green(0), 0);

        EXPECT_EQ(worker.get_max_green(0), 0);

        EXPECT_EQ(worker.get_yellow_duration(0), 0);

        EXPECT_EQ(worker.get_red_clearance(0), 0);

        EXPECT_EQ(worker.get_red_duration(0), 0);

        EXPECT_TRUE(worker.get_following_phases(0).empty());

        EXPECT_TRUE(worker.phase_seq(1).empty());

        EXPECT_TRUE(worker.get_concurrent_phases(1).empty());

    }
}
