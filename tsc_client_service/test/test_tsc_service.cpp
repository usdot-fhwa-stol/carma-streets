#include <gtest/gtest.h>
#include <tsc_service.h>

#include <control_tsc_state.h>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>

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

class mock_snmp_client : public snmp_client
{
    
    public:
    mock_snmp_client(const std::string& ip, const int& port) : snmp_client(ip, port){}

    mock_snmp_client(mock_snmp_client& t, const std::string ip = "", const int port = 0): snmp_client(ip, port){}
    virtual ~mock_snmp_client(void){};
    MOCK_METHOD3(process_snmp_request, bool(const std::string& input_oid, const request_type& request_type,snmp_response_obj& val));
    
};

// Test Fixure to unit test methods requiring access to private members. These tests need to be added as Friend Tests to class
TEST(traffic_signal_controller_service, test_produce_spat_json_timeout) {
    tsc_service service;
    service.initialize_spat("test_intersection",1234,std::unordered_map<int,int>{
                {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}} );
    ASSERT_TRUE(service.initialize_kafka_producer("127.0.0.1:9092", "modified_spat", service.spat_producer));
    ASSERT_TRUE(service.initialize_spat_worker("127.0.0.1",3456,2,false));
    service.produce_spat_json();
}

TEST(traffic_signal_controller_service, test_tsc_control){
    
    std::string dummy_ip = "192.168.10.10";
    int dummy_port = 601;
    mock_snmp_client mock_client_worker(dummy_ip, dummy_port);

    auto unique_client = std::make_unique<mock_snmp_client>(mock_client_worker);
    using testing::_;
    EXPECT_CALL(*unique_client, process_snmp_request(_,_,_) )
        .WillRepeatedly(testing::DoAll(testing::Return(true)));
    
    // Define command 
    snmp_response_obj set_val;
    set_val.type = snmp_response_obj::response_type::INTEGER;
    uint64_t start_time = 0;

    std::shared_ptr<mock_snmp_client> shared_client = std::move(unique_client);

    tsc_control_struct::control_type control_type = tsc_control_struct::control_type::Hold;
    tsc_control_struct hold_command(shared_client, start_time, control_type, 0);
    tsc_service service;
    std::queue<tsc_control_struct> tsc_set_command_queue;
    tsc_set_command_queue.push(hold_command);
    service.tsc_set_command_queue_ = tsc_set_command_queue;
    EXPECT_THROW(service.set_tsc_hold_and_omit(), control_tsc_state_exception);
    
    start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 100;
    tsc_control_struct hold_command_2(shared_client, start_time, control_type, 0);
    std::queue<tsc_control_struct> tsc_set_command_queue_2;
    tsc_set_command_queue_2.push(hold_command_2);
    service.tsc_set_command_queue_ = tsc_set_command_queue_2;
    EXPECT_NO_THROW(service.set_tsc_hold_and_omit());

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
    EXPECT_EQ(service.tsc_config_producer_counter_, 10);
}

}
