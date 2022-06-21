#include <gtest/gtest.h>
#include <iostream>
#include "snmp_client.h"

TEST(traffic_signal_controller_service, test_process_snmp_get_request)
{   
    
    std::string dummy_ip = "192.168.10.10";
    int dummy_port = 601;

    TrafficSignalControllerService worker(dummy_ip, dummy_port);
    
    std::string test_oid = "1.3.6.1.4.1.2021.11.9";

    // Expect get call to fail since it's looking up invalid OID
    EXPECT_EQ(worker.process_snmp_get_request(test_oid), -1);

}

TEST(traffic_signal_controller_service, test_process_snmp_set_request){

    std::string dummy_ip = "192.168.10.10";
    int dummy_port = 601;
    std::string test_oid = "1.3.6.1.4.1.2021.11.9";
    
    TrafficSignalControllerService worker(dummy_ip, dummy_port);
    int set_value = 10;
    // Expect set call to fail since we're trying to set invalid OID
    EXPECT_FALSE(worker.process_snmp_set_request(test_oid, set_value));
}
