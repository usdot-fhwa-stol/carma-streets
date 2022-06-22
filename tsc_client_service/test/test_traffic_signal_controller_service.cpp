#include <gtest/gtest.h>
#include <iostream>
#include "snmp_client.h"

TEST(traffic_signal_controller_service, test_process_snmp_get_request)
{   
    streets_service::streets_configuration::initialize_logger();
    std::string dummy_ip = "192.168.10.10";
    int dummy_port = 601;

    TrafficSignalControllerService worker(dummy_ip, dummy_port);
    
    std::string test_oid = "1.3.6.1.4.1.2021.11.9";
    int integer_response = 0;
    // Expect get call to fail since we're communicating with invalid host
    EXPECT_FALSE(worker.process_snmp_get_request(test_oid, integer_response));
    EXPECT_EQ(integer_response, 0);

    // Test invalid OID
    test_oid = "-1";
    EXPECT_FALSE(worker.process_snmp_get_request(test_oid, integer_response));

    // Test log_error
    std::string request_type = "GET";
    int status = STAT_TIMEOUT;
    worker.log_error(status, request_type);

    status = -7; //Random error value
    worker.log_error(status, request_type);

}

TEST(traffic_signal_controller_service, test_process_snmp_set_request){

    std::string dummy_ip = "192.168.10.10";
    int dummy_port = 601;
    std::string test_oid = "1.3.6.1.4.1.2021.11.9";
    
    TrafficSignalControllerService worker(dummy_ip, dummy_port);
    int set_value = 10;
    // Expect set call to fail since we're communicating with invalid host
    EXPECT_FALSE(worker.process_snmp_set_request(test_oid, set_value));

    // Test invalid OID
    test_oid = "-1";
    EXPECT_FALSE(worker.process_snmp_set_request(test_oid, set_value));

    // Test log_error
    int status = STAT_TIMEOUT;
    std::string request_type = "SET";
    
    status = -7; //Random error value
    worker.log_error(status, request_type);
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}