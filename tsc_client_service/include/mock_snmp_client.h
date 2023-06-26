#pragma once 
#include "snmp_client.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h> 

namespace traffic_signal_controller_service {
    /**
     * @brief Mock SNMP client used for unit testing using gmock. For documentation using gmock mocks 
     * (https://google.github.io/googletest/gmock_for_dummies.html).
     * 
     * @author Paul Bourelly
     */
    class mock_snmp_client : public snmp_client {
        public:
            /**
             * @brief Mock constructor with all default parameters. Can be used as an default constructor.
             * 
             * @param ip 
             * @param port 
             */
            mock_snmp_client(const std::string& ip = "", const int &port = 0 ) : snmp_client(ip, port){};
            ~mock_snmp_client() = default;
            MOCK_METHOD(bool, process_snmp_request, (const std::string &input_oid, const streets_snmp_cmd::REQUEST_TYPE &request_type, streets_snmp_cmd::snmp_response_obj &val), (override));

    };
}