#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "monitor_tsc_state.h"

using namespace tsc_service_mock{
    class mock_tsc_state : public tsc_state
    {
        
        public:
        tsc_state(std::shared_ptr<snmp_client> snmp_client) : tsc_state(std::shared_ptr<snmp_client> snmp_client){}
        virtual ~tsc_state(void){};
        MOCK_METHOD3(process_snmp_request, bool(const std::string& input_oid, const request_type& request_type,snmp_response_obj& val));
        
    };   
}