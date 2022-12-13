#include <gtest/gtest.h>
#include <tsc_service.h>

#include <control_tsc_state.h>
#include <mock_snmp_client.h>
#include <mock_kafka_producer_worker.h>
#include <mock_kafka_consumer_worker.h>

using testing::_;
using testing::Return;
using testing::Throw;
using testing::DoDefault;

namespace traffic_signal_controller_service
{

    class tsc_service_test: public ::testing::Test { 
        public:

            tsc_service_test( ) { 
            // initialization code here
            } 

            void SetUp( ) { 
                // code here will execute just before the test ensues 
                spat_producer = std::make_shared<kafka_clients::mock_kafka_producer_worker>();
                tsc_config_producer = std::make_shared<kafka_clients::mock_kafka_producer_worker>();
                dpp_consumer = std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
                snmp_client =  std::make_shared<mock_snmp_client>();
                service.spat_producer = spat_producer;
                service.tsc_config_producer = tsc_config_producer;
                service.desired_phase_plan_consumer = dpp_consumer;
                service.snmp_client_ptr = snmp_client;

                EXPECT_CALL(*spat_producer, stop()).Times(1);
                EXPECT_CALL(*tsc_config_producer, stop()).Times(1);
                EXPECT_CALL(*dpp_consumer, stop()).Times(1);

            }

            void TearDown( ) { 
                // code here will be called just after the test completes
                // ok to through exceptions from here if need be
            }

            ~tsc_service_test( )  { 
                // cleanup any pending stuff, but no exceptions allowed
            }
            tsc_service service;
            std::shared_ptr<kafka_clients::mock_kafka_producer_worker> spat_producer;
            std::shared_ptr<kafka_clients::mock_kafka_producer_worker> tsc_config_producer;
            std::shared_ptr<kafka_clients::mock_kafka_consumer_worker> dpp_consumer;
            std::shared_ptr<mock_snmp_client> snmp_client;
    // put in any custom data members that you need 
    };
    TEST_F(tsc_service_test, test_initialization) {
        ASSERT_FALSE(service.initialize());
    }

    TEST_F(tsc_service_test, test_init_intersection_client) {
        ASSERT_FALSE(service.initialize_intersection_client());
    }

    TEST_F(tsc_service_test, test_init_spat) {
        service.initialize_spat("test_intersection",1234,std::unordered_map<int,int>{
                    {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}} );
    }

    TEST_F(tsc_service_test, test_init_snmp_client) {
        tsc_service service;
        ASSERT_TRUE(service.initialize_snmp_client("192.90.50.124",12345,"public",2,2 ));
        ASSERT_FALSE(service.enable_spat());

    }


    // Test Fixure to unit test methods requiring access to private members. These tests need to be added as Friend Tests to class
    TEST_F(tsc_service_test, test_produce_spat_json_timeout) {

        service.initialize_spat("test_intersection",1234,std::unordered_map<int,int>{
                    {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}} );
        ASSERT_TRUE(service.initialize_spat_worker("127.0.0.1",3456,2,false));
        service.produce_spat_json();
    }

    TEST_F(tsc_service_test, test_tsc_control){
        
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        

        EXPECT_CALL(*snmp_client, process_snmp_request(_,_,_) )
            .WillRepeatedly(testing::DoAll(testing::Return(true)));
        
        // Define command 
        snmp_response_obj set_val;
        set_val.type = snmp_response_obj::response_type::INTEGER;
        uint64_t start_time = 0;

        snmp_cmd_struct::control_type control_type = snmp_cmd_struct::control_type::Hold;
        snmp_cmd_struct hold_command(snmp_client, start_time, control_type, 0);
        std::queue<snmp_cmd_struct> tsc_set_command_queue;
        tsc_set_command_queue.push(hold_command);
        service.tsc_set_command_queue_ = tsc_set_command_queue;
        EXPECT_THROW(service.set_tsc_hold_and_omit(), control_tsc_state_exception);

        // Test control_tsc_phases
        EXPECT_THROW(service.control_tsc_phases(), control_tsc_state_exception);
        
        start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 100;
        snmp_cmd_struct hold_command_2(snmp_client, start_time, control_type, 0);
        std::queue<snmp_cmd_struct> tsc_set_command_queue_2;
        tsc_set_command_queue_2.push(hold_command_2);
        service.tsc_set_command_queue_ = tsc_set_command_queue_2;
        EXPECT_NO_THROW(service.set_tsc_hold_and_omit());

    }

    TEST_F(tsc_service_test, test_produce_tsc_config_json_timeout) {
        EXPECT_CALL(*tsc_config_producer,is_running()).Times(2).WillRepeatedly(Return(true));
        EXPECT_CALL(*tsc_config_producer, send(_)).Times(2).WillOnce(DoDefault()).WillRepeatedly(Throw(
            streets_tsc_configuration::tsc_configuration_state_exception("Some exception occured")));
        SPDLOG_INFO("Methods mocked");
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

