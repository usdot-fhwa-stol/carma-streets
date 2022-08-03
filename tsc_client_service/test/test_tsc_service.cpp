#include <gtest/gtest.h>
#include <tsc_service.h>

using namespace traffic_signal_controller_service; 

TEST(test_tsc_service, test_initialization) {
    tsc_service service;
    ASSERT_FALSE(service.initialize());
    // Allow for destructor to run before other test cases
    sleep(1);
}

TEST(test_tsc_service, test_init_intersection_client) {
    tsc_service service;
    ASSERT_FALSE(service.initialize_intersection_client());
}

TEST(test_tsc_service, test_init_spat) {
    tsc_service service;
    service.initialize_spat("test_intersection",1234,std::unordered_map<int,int>{
                {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}} );
}

TEST(test_tsc_service, test_init_spat_worker) {
    tsc_service service;
    ASSERT_TRUE(service.initialize_spat_worker("127.0.0.1",3456,2,false));
}

TEST(test_tsc_service, test_init_snmp_client) {
    tsc_service service;
    ASSERT_TRUE(service.initialize_snmp_client("192.90.50.124",12345,"public",2,2 ));
    ASSERT_FALSE(service.enable_spat());

}
