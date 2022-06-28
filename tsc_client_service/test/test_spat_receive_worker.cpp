#include <gtest/gtest.h>
#include <iostream>
#include "snmp_client.h"
#include "spat_receive_worker.h"

namespace traffic_signal_controller_service
{
    TEST(spat_receive_worker, test_enable_spat)
    {   
        std::string dummy_ip = "192.168.120.57";
        int dummy_port = 6054;

        snmp_client worker(dummy_ip, dummy_port);
        
        std::string enable_spat_oid_wrong = "1.3.6.1.4.1.1206.3.5.2.9.49.1.0";
        std::string request_type = "SET";
        int64_t enable_spat_value = 2;
        // Expect set to return false with invalid enable spat OID
        EXPECT_FALSE(worker.process_snmp_request(enable_spat_oid_wrong, request_type, enable_spat_value));
    }

    TEST(spat_receive_worker, test_create_socket)
    {
        std::string tsc_ip_bad = "192.168.120.51";
        int tsc_port = 6053;
        int tsc_timeout = 10;

        SpatWorker spatWorker(tsc_ip_bad, tsc_port, tsc_timeout);
        spatWorker.createSocket();

        // Expect false with wrong tsc ip
        EXPECT_FALSE(spatWorker.getSocketStatus());
    }
    
}