#include <gtest/gtest.h>
#include <iostream>
#include "snmp_client.h"

namespace traffic_signal_controller_service
{
    TEST(traffic_signal_controller_service, test_process_snmp_request)
    {
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        std::string test_oid = "1.3.6.1.4.1.2021.11.9";

        snmp_client worker(dummy_ip, dummy_port);

        // Test GET
        request_type request_type = request_type::GET;
        snmp_response_obj int_response;
        int_response.type = snmp_response_obj::response_type::INTEGER;
        int_response.val_int = 0;
        // Expect get call to fail since we're communicating with invalid host
        EXPECT_FALSE(worker.process_snmp_request(test_oid, request_type, int_response));
        EXPECT_EQ(int_response.val_int, 0);

        // Test invalid OID
        test_oid = "-1";
        EXPECT_FALSE(worker.process_snmp_request(test_oid, request_type, int_response));

        // Test log_error
        request_type = request_type::GET;
        snmp_pdu *response = nullptr;
        int status = STAT_TIMEOUT;
        worker.log_error(status, request_type, response);

        status = -7; //Random error value
        worker.log_error(status, request_type, response);

        // Test SET
        request_type = request_type::SET;
        snmp_response_obj set_value;
        int_response.type = snmp_response_obj::response_type::INTEGER;
        int_response.val_int = 10;
        // Expect set call to fail since we're communicating with invalid host
        EXPECT_FALSE(worker.process_snmp_request(test_oid, request_type, set_value));

        // Test log_error
        status = STAT_TIMEOUT;
        request_type = request_type::SET;
        
        status = -7; //Random error value
        worker.log_error(status, request_type,response);

        // Invalid Request type
        request_type = request_type::OTHER;
        EXPECT_FALSE(worker.process_snmp_request(test_oid, request_type, set_value));
        
    }

}