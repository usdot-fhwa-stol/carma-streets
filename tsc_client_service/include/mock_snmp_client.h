#pragma once 
#include "snmp_client.h" 

namespace traffic_signal_controller_service {
    class mock_snmp_client : public snmp_client {
        public:
            mock_snmp_client(const std::string& ip = "", const int &port = 0 ) : snmp_client(ip, port){};
            ~mock_snmp_client() = default;
            MOCK_METHOD(bool, process_snmp_request, (const std::string &input_oid, const request_type &request_type, snmp_response_obj &val), (override));

    };
}