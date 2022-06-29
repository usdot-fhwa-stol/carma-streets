#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include "snmp_client.h"
#include "spat_worker.h"
#include "spat_worker_exception.h"

namespace traffic_signal_controller_service
{
    /**
     * @brief SNMP required to enable spat using wrong OID should fail.
     * 
     */
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
    /**
     * @brief Attempt to listen on a IP that is not the host of the service should fail.
     * 
     */
    TEST(spat_receive_worker, test_create_socket)
    {
        std::string tsc_ip_bad = "192.168.120.51";
        int tsc_port = 6053;
        int tsc_timeout = 10;

        spat_worker worker(tsc_ip_bad, tsc_port, tsc_timeout);
        try{
            worker.listen_udp_spat();
        }
        catch( const spat_worker_exception &e){
            ASSERT_STREQ( e.what(), "Failed to bind to socket");
        }
        catch( ... ) {
            __assert_fail;
        }

        
    }
    /**
     * @brief Unit test to test UDP socket timeout parameter. Test should timeout
     * after 5 seconds of not receiving data
     * 
     */
    TEST(spat_receive_worker, test_create_socket_timeout)
    {
        std::string tsc_ip_bad = "127.0.0.1";
        int tsc_port = 6053;
        int tsc_timeout = 5;

        spat_worker worker(tsc_ip_bad, tsc_port, tsc_timeout);

        try{
            worker.listen_udp_spat();
        }
        catch( const spat_worker_exception &e){
            ASSERT_STREQ( e.what(), "Timeout of 5 seconds has elapsed. Closing SPaT Work UDP Socket");
        }
        catch( ... ) {
            __assert_fail;
        }
        
    }

    /**
     * @brief Unit test to test UDP socket timeout parameter. Test should timeout
     * after 5 seconds of not receiving data
     * 
     */
    TEST(spat_receive_worker, test_invalid_ip)
    {
        std::string tsc_ip_bad = "asdhas.asd";
        int tsc_port = 6053;
        int tsc_timeout = 5;

        spat_worker worker(tsc_ip_bad, tsc_port, tsc_timeout);
        try{
            worker.listen_udp_spat();
        }
        catch( const spat_worker_exception &e){
            ASSERT_STREQ( e.what(), "Failed to get addr info for the tsc_service instance");
        }
        catch( ... ) {
            __assert_fail;
        }
        
    }  
}