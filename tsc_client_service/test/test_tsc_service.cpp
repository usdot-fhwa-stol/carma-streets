#include <gtest/gtest.h>
#include <tsc_service.h>

using namespace traffic_signal_controller_service; 

    class test_tsc_service : public ::testing::Test{
        private:
        protected:
             /**
             * @brief Test Setup method run before each test.
             * 
             */
            void SetUp() override {
            
            }
            /**
             * @brief Test TearDown method run after each test.
             * 
             */
            void TearDown() override{
            }
        
    };


TEST_F(test_tsc_service, test_initialization) {
    tsc_service service;
    ASSERT_FALSE(service.initialize());
    // Allow kafka client and udp listener socket destructor to run before next test case.
    sleep(1);
}

TEST_F(test_tsc_service, test_init_intersection_client) {
    tsc_service service;
    ASSERT_FALSE(service.initialize_intersection_client());
}

TEST_F(test_tsc_service, test_init_spat) {
    tsc_service service;
    service.initialize_spat("test_intersection",1234,std::unordered_map<int,int>{
                {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}} );
}

TEST_F(test_tsc_service, test_init_snmp_client) {
    tsc_service service;
    ASSERT_TRUE(service.initialize_snmp_client("192.90.50.124",12345,"public",2,2 ));
    ASSERT_FALSE(service.enable_spat());

}

namespace traffic_signal_controller_service {

// Test Fixure to unit test methods requiring access to private members. These tests need to be added as Friend Tests to class
TEST(traffic_signal_controller_service, test_produce_spat_json_timeout) {
    tsc_service service;
    service.initialize_spat("test_intersection",1234,std::unordered_map<int,int>{
                {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}} );
    ASSERT_TRUE(service.initialize_kafka_producer("127.0.0.1:9092", "modified_spat", service.spat_producer));
    ASSERT_TRUE(service.initialize_spat_worker("127.0.0.1",3456,2,false));
    service.produce_spat_json();
}

TEST(traffic_signal_controller_service, test_produce_tsc_config_json_timeout) {
    tsc_service service;
    ASSERT_TRUE(service.initialize_kafka_producer("127.0.0.1:9092", "tsc_config_state", service.tsc_config_producer));
    streets_tsc_configuration::tsc_configuration_state state;

    streets_tsc_configuration::signal_group_configuration signal_group_1;
    signal_group_1.signal_group_id = 1;
    signal_group_1.yellow_change_duration = 10;
    signal_group_1.red_clearance = 5;
    signal_group_1.concurrent_signal_groups = {5,6};
    state.tsc_config_list.push_back(signal_group_1);

    service.tsc_config_state_ptr = std::make_shared<streets_tsc_configuration::tsc_configuration_state>(state);
    service.produce_tsc_config_json();
}

}
