#include <gtest/gtest.h>
#include <iostream>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>

#include "ntcip_oids.h"
#include "control_tsc_state.h"
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

    TEST(traffic_signal_controller_service, test_control_tsc_state)
    {
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        mock_snmp_client mock_client_worker(dummy_ip, dummy_port);

        auto unique_client = std::make_unique<mock_snmp_client>(mock_client_worker);

        // std::shared_ptr<mock_snmp_client> shared_client = std::move(unique_client);
        // std::shared_ptr<streets_desired_phase_plan::streets_desired_phase_plan>  test_desired_phase_plan;
        // std::unordered_map<int, int> signal_group_2ped_phase_map;
        // traffic_signal_controller_service::control_tsc_state worker(shared_client, signal_group_2ped_phase_map, test_desired_phase_plan);

        // Test Hold and Omit for phases 2 and 6

        // Hold- only 2 and 6 should be 1 -> 00100010 (decimal 34)

        // uint8_t hold_val = 0;
        // hold_val |= (1 << 1);
        // hold_val |= (1 << 5);
        // std::cout<<"Hold val: "<< int64_t(hold_val)<<std::endl;

        // // Omit - only 2 and 6 should be 0 -> 11011101 (decimal 221)

        // uint8_t omit_val = 255;
        // omit_val &= ~(1 << 1);
        // omit_val &= ~(1 << 5);
        // std::cout<<"Omit val: "<<int64_t(omit_val)<<std::endl;
       
    }

}