#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include "snmp_client.h"
#include "udp_socket_listener.h"
#include "ntcip_oids.h"

namespace traffic_signal_controller_service
{

    /**
     * @brief Attempt to listen on a IP that is not the host of the service should fail.
     * 
     */
    TEST(udp_socket_listener_test, test_create_socket)
    {
        std::string tsc_ip_bad = "192.168.120.51";
        int tsc_port = 6053;
        int tsc_timeout = 3;

        udp_socket_listener listener(tsc_ip_bad, tsc_port, tsc_timeout);
        bool initialized = listener.initialize();
        ASSERT_FALSE( initialized );

    }
    /**
     * @brief Test UDP socket timeout parameter. Test should timeout
     * after 2 seconds of not receiving data
     * 
     */
    TEST(udp_socket_listener_test, test_create_socket_timeout)
    {
        std::string tsc_ip_bad = "127.0.0.1";
        int tsc_port = 6053;
        int tsc_timeout = 2;

        udp_socket_listener listener(tsc_ip_bad, tsc_port, tsc_timeout);
        bool initialized = listener.initialize();
        ASSERT_TRUE( initialized );
        try{
            listener.receive();
        }
        catch( const udp_socket_listener_exception &e){
            ASSERT_STREQ( e.what(), "Timeout of 2 seconds has elapsed. Closing SPaT Work UDP Socket");
        }
        catch( ... ) {
            __assert_fail;
        }

    }

    /**
     * @brief Test UDP socket with invaild host address
     * 
     */
    TEST(udp_socket_listener_test, test_invalid_ip)
    {
        std::string tsc_ip_bad = "asdhas.asd";
        int tsc_port = 6053;
        int tsc_timeout = 2;

        udp_socket_listener listener(tsc_ip_bad, tsc_port, tsc_timeout);
        bool initialized = listener.initialize();
        ASSERT_FALSE( initialized );
        
        
    }  
}