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
using testing::SetArgReferee;

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
                mock_snmp =  std::make_shared<mock_snmp_client>();
                service.spat_producer = spat_producer;
                service.tsc_config_producer = tsc_config_producer;
                service.desired_phase_plan_consumer = dpp_consumer;
                service.snmp_client_ptr = mock_snmp;
                setenv("SIMULATION_MODE", "FALSE", 1);
                setenv("CONFIG_FILE_PATH", "../manifest.json", 1);


            }
            void mock_tsc_ntcip() {
            // gmock SNMP response ---------------------------------------------------------------------------------------------------------------------
            // Test get max channels
            snmp_response_obj max_channels_in_tsc;
            max_channels_in_tsc.val_int = 16;
            max_channels_in_tsc.type = snmp_response_obj::response_type::INTEGER;

            request_type request_type = request_type::GET;

            EXPECT_CALL( *mock_snmp, process_snmp_request(ntcip_oids::MAX_CHANNELS, request_type , _) ).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(max_channels_in_tsc), 
                Return(true)));

            snmp_response_obj max_rings_in_tsc;
            max_rings_in_tsc.val_int = 4;
            max_rings_in_tsc.type = snmp_response_obj::response_type::INTEGER;
            EXPECT_CALL( *mock_snmp, process_snmp_request(ntcip_oids::MAX_RINGS, request_type , _) ).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(max_rings_in_tsc), 
                Return(true)));
            // Define Sequence Data
            snmp_response_obj seq_data;
            seq_data.val_string = {char(1),char(2), char(3), char(4)};
            std::string seq_data_ring1_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(1);

            EXPECT_CALL(*mock_snmp, process_snmp_request(seq_data_ring1_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(seq_data), 
                Return(true)));

            seq_data.val_string = {char(5),char(6), char(7), char(8)};
            std::string seq_data_ring2_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(2);
            EXPECT_CALL(*mock_snmp, process_snmp_request(seq_data_ring2_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(seq_data), 
                Return(true)));

            seq_data.val_string = {};
            std::string seq_data_ring3_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(3);
            EXPECT_CALL(*mock_snmp, process_snmp_request(seq_data_ring3_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(seq_data), 
                Return(true)));

            std::string seq_data_ring4_oid = ntcip_oids::SEQUENCE_DATA + "." + "1" + "." + std::to_string(4);
            EXPECT_CALL(*mock_snmp, process_snmp_request(seq_data_ring4_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                SetArgReferee<2>(seq_data), 
                Return(true)));
            //get_vehicle_phase channel
            for(int i = 1; i <= max_channels_in_tsc.val_int; ++i){
                // phase number
                // ring 1 ->  | 1 | 2 || 3 | 4 ||
                // ring 2 ->  | 5 | 6 || 7 | 8 ||
                // signal group
                // ring 1 ->  | 1 | 2 || 3 | 4 ||
                // ring 2 ->  | 5 | 6 || 7 | 8 ||
                //
                switch(i) {
                    case 1: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 2;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 1;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true)));
                        break;
                    }
                    case 2: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 2;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 2;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true)));
                        break;
                    }
                    case 3: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 2;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 3;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true)));
                        break;
                    }
                    case 4: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 2;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 4;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true)));
                        break;
                    }
                    case 5: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 2;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 5;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true)));
                        break;
                    }
                    case 6: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 2;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 6;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true)));
                        break;
                    }
                    case 7: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 2;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 7;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true)));
                        break;
                    }
                    case 8: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 2;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 8;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true))); 
                        break;
                    }
                    default: {
                        snmp_response_obj channel_control_resp;
                        channel_control_resp.val_int = 0;
                        std::string channel_control_oid = ntcip_oids::CHANNEL_CONTROL_TYPE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(channel_control_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(channel_control_resp),
                            Return(true)));

                        // Define Control Source
                        // Note this OID is not actually called for any non vehicle/pedestrian phases which is why the Times() assertion 
                        // is not included
                        snmp_response_obj control_source_resp;
                        control_source_resp.val_int = 0;
                        std::string control_source_oid = ntcip_oids::CHANNEL_CONTROL_SOURCE_PARAMETER + "." + std::to_string(i);
                        EXPECT_CALL(*mock_snmp, process_snmp_request(control_source_oid, request_type , _) ).WillRepeatedly(testing::DoAll(
                            SetArgReferee<2>(control_source_resp), 
                            Return(true)));
                    }
                }
                
            }
            for(int i = 1; i <= 8; i++) {
                

                // Define get min green
                std::string min_green_oid = ntcip_oids::MINIMUM_GREEN + "." + std::to_string(i);
                snmp_response_obj min_green;
                min_green.val_int = 5;
                EXPECT_CALL(*mock_snmp, process_snmp_request(min_green_oid , request_type , _) ).WillRepeatedly(testing::DoAll(
                    SetArgReferee<2>(min_green), 
                    Return(true)));
                

                // Define get max green
                std::string max_green_oid = ntcip_oids::MAXIMUM_GREEN + "." + std::to_string(i);
                snmp_response_obj max_green;
                max_green.val_int = 300;
                EXPECT_CALL(*mock_snmp, process_snmp_request(max_green_oid , request_type , _) ).WillRepeatedly(testing::DoAll(
                    SetArgReferee<2>(max_green), 
                    Return(true)));


                // Define get yellow Duration
                std::string yellow_oid = ntcip_oids::YELLOW_CHANGE_PARAMETER + "." + std::to_string(i);
                snmp_response_obj yellow_duration;
                yellow_duration.val_int = 10;
                EXPECT_CALL(*mock_snmp, process_snmp_request(yellow_oid , request_type , _) ).WillRepeatedly(testing::DoAll(
                    SetArgReferee<2>(yellow_duration), 
                    Return(true)));

                

                // Define red clearance
                std::string red_clearance_oid = ntcip_oids::RED_CLEAR_PARAMETER + "." + std::to_string(i);
                snmp_response_obj red_clearance_duration;
                red_clearance_duration.val_int = 15;
                EXPECT_CALL(*mock_snmp, process_snmp_request(red_clearance_oid , request_type , _) ).WillRepeatedly(testing::DoAll(
                    SetArgReferee<2>(red_clearance_duration), 
                    Return(true)));


                //Define get concurrent phases
                std::string concurrent_phase_oid = ntcip_oids::PHASE_CONCURRENCY + "." + std::to_string(i);
                snmp_response_obj concurrent_phase_resp;
                if ( i == 1 || i == 2) {
                    concurrent_phase_resp.val_string = {char(5), char(6)};
                }
                else if (i == 3|| i == 4) {
                    concurrent_phase_resp.val_string = {char(7), char(8)};
                }
                else if (i == 5|| i == 6) {
                    concurrent_phase_resp.val_string = {char(1), char(2)};
                }
                // if 7 || 8
                else {
                concurrent_phase_resp.val_string = {char(3), char(4)}; 
                }
                EXPECT_CALL(*mock_snmp, process_snmp_request(concurrent_phase_oid , request_type , _) ).WillRepeatedly(testing::DoAll(
                    SetArgReferee<2>(concurrent_phase_resp), 
                    Return(true)));
            }
            // gmock SNMP response END ----------------------------------------------------------------------------------------------------------------------------------

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
            std::shared_ptr<mock_snmp_client> mock_snmp;
    // put in any custom data members that you need 
    };
    TEST_F(tsc_service_test, test_initialization_mock_everything_except_intersection_client) {
        mock_tsc_ntcip();
        ASSERT_FALSE(service.initialize());
        // TODO Replace this unit test coverage after fixing streets_configuration_singleton
        // tsc_config_producer = nullptr;
        // ASSERT_FALSE(service.initialize());
        // mock_snmp = nullptr;
        // ASSERT_FALSE(service.initialize());
        // dpp_consumer = nullptr;
        // ASSERT_FALSE(service.initialize());
        // spat_producer  = nullptr;
        // ASSERT_FALSE(service.initialize());
    }

    TEST_F(tsc_service_test, test_init_intersection_client) {
      
        // Initialize clock singleton in realtime mode
        streets_service::streets_clock_singleton::create(false);
        ASSERT_FALSE(service.initialize_intersection_client());
    }
    // TODO: Restore this test case by fixing streets_configuration singleton
    // TEST_F(tsc_service_test, test_init_kafka_consumer_producer) {
    //     std::string test = "test";
    //     service.initialize();
    //     ASSERT_TRUE(service.initialize_kafka_consumer(test , service.desired_phase_plan_consumer ));
    //     ASSERT_TRUE(service.initialize_kafka_producer(test, service.spat_producer));
    // }

    TEST_F(tsc_service_test, test_init_spat) {
        
        // Initialize clock singleton in realtime mode
        streets_service::streets_clock_singleton::create(false);
        service.initialize_spat("test_intersection",1234,std::unordered_map<int,int>{
                    {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}} );
    }


    TEST_F(tsc_service_test, test_init_snmp_client) {

        // Initialize clock singleton in realtime mode
        streets_service::streets_clock_singleton::create(false);
        ASSERT_TRUE(service.initialize_snmp_client("192.90.50.124",12345,"public",2,2 ));
        ASSERT_FALSE(service.enable_spat());

    }


    // Test Fixure to unit test methods requiring access to private members. These tests need to be added as Friend Tests to class
    TEST_F(tsc_service_test, test_produce_spat_json_timeout) {
        
        // Initialize clock singleton in realtime mode
        streets_service::streets_clock_singleton::create(false);
        service.initialize_spat("test_intersection",1234,std::unordered_map<int,int>{
                    {1,8},{2,7},{3,6},{4,5},{5,4},{6,3},{7,2},{8,1}} );
        ASSERT_TRUE(service.initialize_spat_worker("127.0.0.1",3456,2,false));
        service.produce_spat_json();
    }

    TEST_F(tsc_service_test, test_tsc_control){
        
        // Initialize clock singleton in realtime mode
        streets_service::streets_clock_singleton::create(false);
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        

        EXPECT_CALL(*mock_snmp, process_snmp_request(_,_,_) )
            .WillRepeatedly(testing::DoAll(testing::Return(true)));
        
        // Define command 
        snmp_response_obj set_val;
        set_val.type = snmp_response_obj::response_type::INTEGER;
        uint64_t start_time = 0;

        snmp_cmd_struct::control_type control_type = snmp_cmd_struct::control_type::Hold;
        snmp_cmd_struct hold_command(mock_snmp, start_time, control_type, 0);
        std::queue<snmp_cmd_struct> tsc_set_command_queue;
        tsc_set_command_queue.push(hold_command);
        service.tsc_set_command_queue_ = tsc_set_command_queue;

        EXPECT_THROW(service.set_tsc_hold_and_omit(), control_tsc_state_exception);
        ASSERT_EQ(1, service.tsc_set_command_queue_.size());
        // Test control_tsc_phases
        service.control_tsc_phases();
        ASSERT_EQ(0, service.tsc_set_command_queue_.size());

        start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 100;
        snmp_cmd_struct hold_command_2(mock_snmp, start_time, control_type, 0);
        std::queue<snmp_cmd_struct> tsc_set_command_queue_2;
        tsc_set_command_queue_2.push(hold_command_2);
        service.tsc_set_command_queue_ = tsc_set_command_queue_2;
        ASSERT_EQ(1, service.tsc_set_command_queue_.size());
        EXPECT_NO_THROW(service.set_tsc_hold_and_omit());
        ASSERT_EQ(0, service.tsc_set_command_queue_.size());

    }

    TEST_F(tsc_service_test, test_log_spat_latency){
        // Initialize clock singleton in realtime mode
        SPDLOG_INFO("IS This happening");
        streets_service::streets_clock_singleton::create(false);
        int count = 0;
        uint64_t spat_latency = 0;
        // 100 ms latency
        uint64_t spat_time_stamp = streets_service::streets_clock_singleton::time_in_ms() - 100;
        service.log_spat_latency(count, spat_latency, spat_time_stamp);
        // Confirm count and latency variables were passed by reference 
        ASSERT_EQ( count, 1);
        // 2ms threshold for system time comparisons
        EXPECT_NEAR( spat_latency, 100, 2);
        // Run 19 more times to get method to print average and reset count and latency.
        for (int i = 0; i < 18 ; i++) {
            service.log_spat_latency(count, spat_latency , spat_time_stamp);
            ASSERT_EQ( count, i+2);
            // 2ms threshold for system time comparisons
            EXPECT_NEAR( spat_latency, 200 + i*100,2);
        }
        service.log_spat_latency(count, spat_latency , spat_time_stamp);
        ASSERT_EQ( count, 0);
        // 2ms threshold for system time comparisons
        EXPECT_NEAR( spat_latency, 0, 2);

    }
    TEST_F(tsc_service_test, test_produce_tsc_config_json_timeout) {

        // Initialize clock singleton in realtime mode
        streets_service::streets_clock_singleton::create(false);
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

    TEST_F(tsc_service_test, test_configure_snmp_cmd_logger)
    {

        // Initialize clock singleton in realtime mode
        streets_service::streets_clock_singleton::create(false);
        service.configure_snmp_cmd_logger();
        auto logger = spdlog::get("snmp_cmd_logger"); 
        EXPECT_TRUE(logger != nullptr);
    }
    // TODO: Restore test by fixting streets_configuration_singleton
    // TEST_F(tsc_service_test, test_initialization_no_mock_response_from_snmp_client) {
    //     ASSERT_FALSE(service.initialize());
    // }
}

