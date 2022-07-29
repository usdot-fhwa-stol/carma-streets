#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include "snmp_client.h"
#include "udp_socket_listener.h"
#include "spat_worker_exception.h"
#include "ntcip_oids.h"

namespace traffic_signal_controller_service
{
    /**
     * @brief SNMP required to enable spat using OID should fail.
     * 
     */
    TEST(spat_receive_client, test_enable_spat)
    {   
        std::string dummy_ip = "192.168.120.57";
        int dummy_port = 6054;

        snmp_client client(dummy_ip, dummy_port);
        
        request_type request_type = request_type::SET;
        snmp_response_obj enable_spat;
        enable_spat.type = snmp_response_obj::response_type::INTEGER;
        enable_spat.val_int = 2;
        
        // Expect set to return false with invalid enable spat OID
        EXPECT_FALSE(client.process_snmp_request(ntcip_oids::ENABLE_SPAT_OID, request_type, enable_spat));
    }
    /**
     * @brief Attempt to listen on a IP that is not the host of the service should fail.
     * 
     */
    TEST(spat_receive_worker, test_create_socket)
    {
        std::string tsc_ip_bad = "192.168.120.51";
        int tsc_port = 6053;
        int tsc_timeout = 3;

        upd_socket_listener listener(tsc_ip_bad, tsc_port, tsc_timeout);
        bool initialized = listener.initialize();
        ASSERT_FALSE( initialized );

    }
    /**
     * @brief Ttest UDP socket timeout parameter. Test should timeout
     * after 2 seconds of not receiving data
     * 
     */
    TEST(spat_receive_worker, test_create_socket_timeout)
    {
        std::string tsc_ip_bad = "127.0.0.1";
        int tsc_port = 6053;
        int tsc_timeout = 2;

        upd_socket_listener listener(tsc_ip_bad, tsc_port, tsc_timeout);
        bool initialized = listener.initialize();
        ASSERT_FALSE( initialized );
    }

    /**
     * @brief Test UDP socket with invaild host address
     * 
     */
    TEST(spat_receive_worker, test_invalid_ip)
    {
        std::string tsc_ip_bad = "asdhas.asd";
        int tsc_port = 6053;
        int tsc_timeout = 2;

        upd_socket_listener listener(tsc_ip_bad, tsc_port, tsc_timeout);
        bool initialized = listener.initialize();
        ASSERT_FALSE( initialized );
        
        
    }  
}