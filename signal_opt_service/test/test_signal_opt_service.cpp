#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "signal_opt_service.h"
#include "mock_kafka_consumer_worker.h"
#include "mock_kafka_producer_worker.h"
#include "movement_group.h"

using testing::_;
using testing::Return;

/**
 * @brief Test signal opt service initialize method.
 */
TEST(signal_opt_service, initialize)
{
    signal_opt_service::signal_opt_service so_service;
    ASSERT_FALSE(so_service.initialize());
    
}
/**
 * @brief Test signal opt service update intersection info method.
 */
TEST(signal_opt_service, update_intersection_info)
{   
    signal_opt_service::signal_opt_service so_service;
    ASSERT_FALSE(so_service.initialize());
    ASSERT_FALSE(so_service.update_intersection_info(1000, 1));
}
namespace signal_opt_service {
/**
 * @brief Test read configuration parameters from manifest.json.
 */
TEST(signal_opt_service, test_read_configuration_params)
{
    signal_opt_service so_service;
    so_service.read_configuration_params();

    ASSERT_EQ( so_service._exp_delta, 60000);
    ASSERT_EQ( so_service._bootstrap_server, "127.0.0.1:9092");
    ASSERT_EQ( so_service._spat_topic_name, "modified_spat");
    ASSERT_EQ( so_service._spat_group_id, "spat_msg_group");
    ASSERT_EQ( so_service._vsi_topic_name, "vehicle_status_intent_output");
    ASSERT_EQ( so_service._vsi_group_id, "vsi_msg_group");
    ASSERT_EQ( so_service._tsc_config_topic_name, "tsc_config_state");
    ASSERT_EQ( so_service._tsc_config_group_id, "tsc_config_group");
    ASSERT_EQ( so_service._dpp_topic_name, "desired_phase_plan");
    ASSERT_EQ( so_service._so_log_path, "../logs/");
    ASSERT_EQ( so_service._so_log_filename, "soLogs");
    ASSERT_EQ( so_service.enable_so_logging, true);
    ASSERT_EQ( so_service.so_sleep_time, 4000);

    ASSERT_EQ( so_service.dpp_config.initial_green_buffer, 2000);
    ASSERT_EQ( so_service.dpp_config.final_green_buffer, 2000);
    ASSERT_EQ( so_service.dpp_config.et_inaccuracy_buffer, 0);
    ASSERT_EQ( so_service.dpp_config.queue_max_time_headway, 0);
    ASSERT_EQ( so_service.dpp_config.so_radius, 200.0);
    ASSERT_EQ( so_service.dpp_config.min_green, 5000);
    ASSERT_EQ( so_service.dpp_config.max_green, 120000);
    ASSERT_EQ( so_service.dpp_config.desired_future_move_group_count, 1);
}

/**
 * @brief Test produce dpp with mock kafka producer.
 */
TEST(signal_opt_service, test_produce_dpp) {
        
    signal_opt_service so_service;
    so_service._so_processing_worker_ptr = std::make_shared<signal_opt_processing_worker>();
    auto mock_dpp_producer = std::make_shared<kafka_clients::mock_kafka_producer_worker>();
    
    // Create mock intersection_info_ptr
    auto intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>();
    std::string json_info = "{\"departure_lanelets\":[{ \"id\":5, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":6, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":7, \"length\":34.130869420842046, \"speed_limit\":11.176 }, { \"id\":8, \"length\":50.123213235343123, \"speed_limit\":11.176 }], \"entry_lanelets\":[{ \"id\":1, \"length\":295.73023157287864, \"speed_limit\":8.0, \"connecting_lanelet_ids\": [9, 10] }, { \"id\":2, \"length\":334.130869411176431136, \"speed_limit\":8.0, \"connecting_lanelet_ids\": [11, 12] }, { \"id\":3, \"length\":341.60952435603712, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [13, 14]}, { \"id\":4, \"length\":353.19846216254821, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [15, 16]}], \"id\":9001, \"link_lanelets\":[{ \"conflict_lanelet_ids\":[ 11, 15, 16 ], \"id\":9, \"length\":15.85409574709938, \"speed_limit\":8.0, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 11 ], \"id\":10, \"length\":16.796388658952235, \"speed_limit\":4.4704, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 9, 10, 13 ], \"id\":11, \"length\":16.043077028554038, \"speed_limit\":8.0, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[ 13 ], \"id\":12, \"length\":10.295559117055083, \"speed_limit\":4.4704, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[ 11, 12, 15 ], \"id\":13, \"length\":15.853947840111768943, \"speed_limit\":8.0, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 15 ], \"id\":14, \"length\":9.744590320260139, \"speed_limit\":4.4704, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 9, 13, 14 ], \"id\":15, \"length\":13.6473819283719203846, \"speed_limit\":8.0, \"signal_group_id\":4 }, { \"conflict_lanelet_ids\":[ 9 ], \"id\":16, \"length\":8.182736100981263, \"speed_limit\":4.4704, \"signal_group_id\":4 }], \"name\":\"WestIntersection\"}";  
    intersection_info_ptr->fromJson(QString::fromStdString(json_info));


    // Create mock spat_ptr
    auto spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
    std::string json_spat = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":0,\"revision\":123,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[9, 10, 11, 12, 13, 14, 15, 16],\"states\":[{\"movement_name\":\"All Directions\",\"signal_group\":1,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}}]},{\"movement_name\":\"All Directions\",\"signal_group\":2,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10000}},{\"event_state\":6,\"timing\":{\"start_time\":10000,\"min_end_time\":10050}}, {\"event_state\":8,\"timing\":{\"start_time\":10050,\"min_end_time\":10080}}, {\"event_state\":3,\"timing\":{\"start_time\":10080,\"min_end_time\":10100}}]},{\"movement_name\":\"All Directions\",\"signal_group\":3,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}}]}, {\"movement_name\":\"All Directions\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    spat_ptr->fromJson(json_spat);

    uint64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>
                                                (std::chrono::system_clock::now().time_since_epoch()).count();
    /** Update the spat timing based on the currect timestamp */
    std::vector<std::vector<std::pair<uint64_t, uint64_t>>> spat_timing;
    std::vector<std::pair<uint64_t, uint64_t>> spat_timing_array;
    
    spat_timing_array.clear();
    spat_timing_array.push_back({current_time - 5000, current_time + 10000});
    spat_timing.push_back(spat_timing_array);
    spat_timing_array.clear();
    spat_timing_array.push_back({current_time - 5000, current_time});
    spat_timing_array.push_back({current_time, current_time + 5000});
    spat_timing_array.push_back({current_time + 5000, current_time + 8000});
    spat_timing_array.push_back({current_time + 8000, current_time + 10000});
    spat_timing.push_back(spat_timing_array);
    spat_timing_array.clear();
    spat_timing_array.push_back({current_time - 5000, current_time + 10000});
    spat_timing.push_back(spat_timing_array);
    spat_timing_array.clear();
    spat_timing_array.push_back({current_time - 5000, current_time + 10000});
    spat_timing.push_back(spat_timing_array);
    int d1_iterator = 0;
    int d2_iterator = 0;
    auto intersection_state = spat_ptr->get_intersection();
    for (auto &state : intersection_state.states) {
        d2_iterator = 0;
        for (auto &state_timing : state.state_time_speed) {
            state_timing.timing.set_start_time(spat_timing[d1_iterator][d2_iterator].first);
            state_timing.timing.set_min_end_time(spat_timing[d1_iterator][d2_iterator].second);
            d2_iterator += 1;
        }
        d1_iterator += 1;
    }
    spat_ptr->set_intersection(intersection_state);

    // Create mock tsc_config_ptr
    auto tsc_config_ptr = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();
    std::string json_tsc_config = "{\"tsc_config_list\":[{\"signal_group_id\":1, \"yellow_change_duration\":3000, \"red_clearance\":2000}, {\"signal_group_id\":2, \"yellow_change_duration\":3000, \"red_clearance\":2000}, {\"signal_group_id\":3, \"yellow_change_duration\":3000, \"red_clearance\":2000}, {\"signal_group_id\":4, \"yellow_change_duration\":3000, \"red_clearance\":2000}]}";
    tsc_config_ptr->fromJson(json_tsc_config);

    // Create a list of movement groups
    auto move_groups_ptr = std::make_shared<streets_signal_optimization::movement_groups>();
    streets_signal_optimization::movement_group mg1;
    mg1.name = "movement_group_1";
    mg1.signal_groups = {1, 3};
    move_groups_ptr->groups.push_back(mg1);
    streets_signal_optimization::movement_group mg2;
    mg2.name = "movement_group_2";
    mg2.signal_groups = {2, 0};
    move_groups_ptr->groups.push_back(mg2);
    streets_signal_optimization::movement_group mg3;
    mg3.name = "movement_group_3";
    mg3.signal_groups = {4, 0};
    move_groups_ptr->groups.push_back(mg3);

    // Creat a mock dpp_config
    streets_signal_optimization::streets_desired_phase_plan_generator_configuration dpp_config;
    dpp_config.initial_green_buffer = 2000;
    dpp_config.final_green_buffer = 2000;
    dpp_config.et_inaccuracy_buffer = 1000;
    dpp_config.queue_max_time_headway = 3000;
    dpp_config.so_radius = 250;
    dpp_config.min_green = 5000;
    dpp_config.max_green = 120000;
    dpp_config.desired_future_move_group_count = 2;

    // Create a mock vehicle_list_ptr
    auto veh_list_ptr = std::make_shared<streets_vehicles::vehicle_list>();
    // Set signalized_status_intent_processor processor for vehicle ist
    veh_list_ptr->set_processor(std::make_shared<streets_vehicles::signalized_status_intent_processor>());
    auto processor = std::dynamic_pointer_cast<streets_vehicles::signalized_status_intent_processor>(veh_list_ptr->get_processor());
    processor->set_timeout(3.154e11);
    
    std::string json_vehicle_1 = "{\"metadata\":{\"timestamp\": " + std::to_string(current_time) + "}, \"payload\": {\"v_id\": \"DOT-507\", \"v_length\": 500, \"min_gap\": 5.0,\"react_t\": 0.5,\"max_accel\": 1.0,\"max_decel\": -1.0,\"cur_speed\": 6.0,\"cur_accel\": 0.0,\"cur_lane_id\": 1,\"cur_ds\": 62.0,\"direction\": \"straight\",\"entry_lane_id\": 1,\"link_lane_id\": 9,\"dest_lane_id\": 5,\"is_allowed\": false,\"depart_pos\": 0,\"est_paths\": [{\"ts\": " + std::to_string(current_time) + ",\"id\": 1,\"ds\": 62.0}]}}";
    std::string json_vehicle_2 = "{\"metadata\":{\"timestamp\": " + std::to_string(current_time) + "}, \"payload\": {\"v_id\": \"DOT-508\", \"v_length\": 500, \"min_gap\": 5.0, \"react_t\": 0.5, \"max_accel\": 1.0,\"max_decel\": -1.0,\"cur_speed\": 6.0,\"cur_accel\": 0.0,\"cur_lane_id\": 4,\"cur_ds\": 94.0,\"direction\": \"straight\",\"entry_lane_id\": 4,\"link_lane_id\": 15,\"dest_lane_id\": 8,\"is_allowed\": false,\"depart_pos\": 0,\"est_paths\": [{\"ts\": " + std::to_string(current_time) + ",\"id\": 4,\"ds\": 94.0}]}}";

    veh_list_ptr->process_update(json_vehicle_1);
    veh_list_ptr->process_update(json_vehicle_2);

    int so_sleep_time = 1000;
    // std::string json_dpp = "{\"timestamp\": 12121212121, \"desired_phase_plan\": [{\"signal_groups\": [2], \"start_time\": 1660747993, \"end_time\": 1660747998}, {\"signal_groups\": [1, 3], \"start_time\": 1660747993, \"end_time\": 1660747998}]}";

    EXPECT_CALL(*mock_dpp_producer,is_running()).Times(3).WillOnce(Return(false))
                                                        .WillOnce(Return(true))
                                                        .WillRepeatedly(Return(false));

    EXPECT_CALL(*mock_dpp_producer, send(_)).Times(1);

    // try to produce chosen dpp while kafka producer is not running.
    so_service.produce_dpp(mock_dpp_producer, 
                            intersection_info_ptr, 
                            veh_list_ptr, 
                            spat_ptr, 
                            tsc_config_ptr, 
                            move_groups_ptr, 
                            dpp_config, 
                            so_sleep_time);

    // produce chosen dpp while kafka producer is running.
    // After sending 1 chosen dpp, kafka producer stops running.
    so_service.produce_dpp(mock_dpp_producer, 
                            intersection_info_ptr, 
                            veh_list_ptr, 
                            spat_ptr, 
                            tsc_config_ptr, 
                            move_groups_ptr, 
                            dpp_config,  
                            so_sleep_time);
}
}

/**
 * @brief Test consume spat with mock kafka consumer.
 */
TEST(signal_opt_service, consume_spat) {
    signal_opt_service::signal_opt_service so_service;
    std::shared_ptr<kafka_clients::mock_kafka_consumer_worker> mock_spat_consumer = std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
    std::shared_ptr<signal_phase_and_timing::spat> spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
    EXPECT_CALL(*mock_spat_consumer,is_running()).Times(5).WillOnce(Return(false))
                                                            .WillOnce(Return(true))
                                                            .WillOnce(Return(true))
                                                            .WillOnce(Return(true))
                                                            .WillRepeatedly(Return(false));
    EXPECT_CALL(*mock_spat_consumer, consume(1000)).Times(3).WillOnce(Return("")).WillOnce(Return("Not JSON")).WillRepeatedly(Return(
        "{"
            "\"timestamp\":0,"
            "\"name\":\"West Intersection\","
            "\"intersections\":["
            "{"
                "\"name\":\"West Intersection\","
                "\"id\":1909,"
                "\"status\":0,"
                "\"revision\":123,"
                "\"moy\":34232,"
                "\"time_stamp\":130,"
                "\"enabled_lanes\":[1,3,5],"
                "\"states\":["
                    "{"
                        "\"movement_name\":\"Right Turn\","
                        "\"signal_group\":4,"
                        "\"state_time_speed\":["
                            "{"
                                "\"event_state\":3,"
                                "\"timing\":{\"start_time\":0,\"min_end_time\":0,\"max_end_time\":0,\"likely_time\":0,\"confidence\":0},"
                                "\"speeds\":[{\"type\":0,\"speed_limit\":4,\"speed_confidence\":1,\"distance\":5,\"class\":5}]"
                            "}],"
                            "\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]"
                    "}],"
                "\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}"));
    // consume spat while kafka consumer is not running.
    so_service.consume_spat(mock_spat_consumer,spat_ptr);
    // consume spat while kafka consumer is running.
    // 1st message is empty
    // 2nd message is invalid json
    // 3rd message is valid
    // After 3rd message kafka consumer stops running.
    so_service.consume_spat(mock_spat_consumer, spat_ptr);

}
/**
 * @brief Test consume tsc config with mock kafka consumer.
 */
TEST(signal_opt_service, consume_tsc_config) {
    signal_opt_service::signal_opt_service so_service;
    std::shared_ptr<kafka_clients::mock_kafka_consumer_worker> mock_tsc_config_consumer = std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
    std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();
    EXPECT_CALL(*mock_tsc_config_consumer,is_running()).Times(5).WillOnce(Return(false))
                                                            .WillOnce(Return(true))
                                                            .WillOnce(Return(true))
                                                            .WillOnce(Return(true))
                                                            .WillRepeatedly(Return(false));
    EXPECT_CALL(*mock_tsc_config_consumer, consume(1000)).Times(3).WillOnce(Return("")).WillOnce(Return("Not JSON")).WillRepeatedly(Return(
        "{"
        "\"tsc_config_list\":["
        "{"
        "    \"signal_group_id\": 1,"
        "    \"yellow_change_duration\":1000,"
        "    \"red_clearance\":500,"
        "    \"concurrent_signal_groups\":[5,6]"
        "},"
        "{"
        "    \"signal_group_id\": 2,"
        "    \"yellow_change_duration\":2000,"
        "    \"red_clearance\":300,"
        "    \"concurrent_signal_groups\":[5,6]"
        "},"
        "{"
        "    \"signal_group_id\": 7,"
        "    \"yellow_change_duration\":2000,"
        "    \"red_clearance\":300"
        "}]"
        "}"));
    // consume tsc configuration while kafka consumer is not running.
    so_service.consume_tsc_config(mock_tsc_config_consumer,tsc_config);
    // consume tsc configuration while kafka consumer is running.
    // 1st message is empty
    // 2nd message is invalid json 
    // 3rd message is valid
    so_service.consume_tsc_config(mock_tsc_config_consumer, tsc_config);
   
}
/**
 * @brief Test consume vsi method with mock kafka consumer.
 */
TEST(signal_opt_service, consume_vsi) {
    signal_opt_service::signal_opt_service so_service;
    std::shared_ptr<kafka_clients::mock_kafka_consumer_worker> mock_vsi_consumer = std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
    std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list = std::make_shared<streets_vehicles::vehicle_list>();
    auto status_intent_processor = std::make_shared<streets_vehicles::signalized_status_intent_processor>();
    vehicle_list->set_processor(status_intent_processor);
    auto cur_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    EXPECT_CALL(*mock_vsi_consumer,is_running()).Times(5).WillOnce(Return(false))
                                                        .WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillOnce(Return(true))
                                                        .WillRepeatedly(Return(false));
    EXPECT_CALL(*mock_vsi_consumer, consume(1000)).Times(3)
            .WillOnce(Return(""))
            .WillOnce(Return("Not JSON"))
            .WillRepeatedly(Return(
                "{ \"metadata\":"
                    "{ \"timestamp\": 1664375832148 },"
                "\"payload\": "
                    "{"
                        "\"v_id\": \"DOT-507\","
                        "\"v_length\": 5," 
                        "\"min_gap\": 15.0,"
                        "\"react_t\": 2,"
                        "\"max_accel\": 5.0,"
                        "\"max_decel\": 5.0,"
                        "\"cur_speed\": 10.0," 
                        "\"cur_accel\": 0.0," 
                        "\"cur_lane_id\": 7," 
                        "\"cur_ds\": 7.0," 
                        "\"direction\": \"straight\","
                        "\"entry_lane_id\": 7,"
                        "\"link_lane_id\": 8," 
                        "\"dest_lane_id\": 9,"
                        "\"is_allowed\": false," 
                        "\"depart_pos\": 1," 
                        "\"est_paths\":" 
                            "["
                                "{" 
                                    "\"ts\": 1623677096200,"
                                    "\"id\": 7,"
                                    "\"ds\": 6.0"
                                "}," 
                                "{" 
                                    "\"ts\": 1623677096400,"
                                    "\"id\": 7," 
                                    "\"ds\": 5.0" 
                                "}," 
                                "{" 
                                    "\"ts\": 1623677096600," 
                                    "\"id\": 7,"
                                    "\"ds\": 4.0" 
                                "}," 
                                "{" 
                                    "\"ts\": 1623677096800," 
                                    "\"id\": 7," 
                                    "\"ds\": 3.0" 
                                "}," 
                                "{" 
                                    "\"ts\": 1623677097000,"
                                    "\"id\": 7,"
                                    "\"ds\": 2.0"
                                "}" 
                            "]" 
                    "}"
                "}"));
    // consume vehicle status and intent while kafka consumer is not runninng.
    so_service.consume_vsi(mock_vsi_consumer, vehicle_list);
    // consume vehicle status and intent while kafka consumer is running.
    // 1st message is empty
    // 2nd message is invalid json
    // 3rd message is valid 
    // After 3rd message kafka consumer stops running.
    so_service.consume_vsi(mock_vsi_consumer, vehicle_list);
}
/**
 * @brief Test populate movement group method with example tsc configurations state.
 */
TEST(signal_opt_service, populate_movement_group) {
    signal_opt_service::signal_opt_service so_service;
    auto tsc_config = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();
    auto m_groups = std::make_shared<streets_signal_optimization::movement_groups>();
    streets_tsc_configuration::signal_group_configuration sig5;
    sig5.signal_group_id = 5;
    sig5.yellow_change_duration = 3000;
    sig5.red_clearance = 2000;
    sig5.concurrent_signal_groups.push_back(6);
    tsc_config->tsc_config_list.push_back(sig5);

    streets_tsc_configuration::signal_group_configuration sig2;
    sig2.signal_group_id = 2;
    sig2.yellow_change_duration = 3000;
    sig2.red_clearance = 2000;
    sig2.concurrent_signal_groups.push_back(4);
    tsc_config->tsc_config_list.push_back(sig2);

    streets_tsc_configuration::signal_group_configuration sig6;
    sig6.signal_group_id = 6;
    sig6.yellow_change_duration = 3000;
    sig6.red_clearance = 2000;
    sig6.concurrent_signal_groups.push_back(5);
    tsc_config->tsc_config_list.push_back(sig6);


    streets_tsc_configuration::signal_group_configuration sig4;
    sig4.signal_group_id = 4;
    sig4.yellow_change_duration = 3000;
    sig4.red_clearance = 2000;
    sig4.concurrent_signal_groups.push_back(2);
    tsc_config->tsc_config_list.push_back(sig4);

    // TSC Configuration
    // barrier = ||
    //ring 1 : 2 || 5
    //ring 2 : 4 || 6
    so_service.populate_movement_groups(m_groups,tsc_config);
    
    ASSERT_EQ(2, m_groups->groups.size());
    ASSERT_EQ(5, m_groups->groups.front().signal_groups.first);
    ASSERT_EQ(6, m_groups->groups.front().signal_groups.second);

    ASSERT_EQ(2, m_groups->groups.back().signal_groups.first);
    ASSERT_EQ(4, m_groups->groups.back().signal_groups.second);


}

/**
 * @brief Test populate movement group method with example tsc configurations state.
 */
TEST(signal_opt_service, populate_movement_group_no_concurrent_sg) {
    signal_opt_service::signal_opt_service so_service;
    auto tsc_config = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();
    auto m_groups = std::make_shared<streets_signal_optimization::movement_groups>();
    streets_tsc_configuration::signal_group_configuration sig5;
    sig5.signal_group_id = 5;
    sig5.yellow_change_duration = 3000;
    sig5.red_clearance = 2000;
    tsc_config->tsc_config_list.push_back(sig5);

    streets_tsc_configuration::signal_group_configuration sig2;
    sig2.signal_group_id = 2;
    sig2.yellow_change_duration = 3000;
    sig2.red_clearance = 2000;
    sig2.concurrent_signal_groups.push_back(4);
    tsc_config->tsc_config_list.push_back(sig2);

    streets_tsc_configuration::signal_group_configuration sig6;
    sig6.signal_group_id = 6;
    sig6.yellow_change_duration = 3000;
    sig6.red_clearance = 2000;
    tsc_config->tsc_config_list.push_back(sig6);


    streets_tsc_configuration::signal_group_configuration sig4;
    sig4.signal_group_id = 4;
    sig4.yellow_change_duration = 3000;
    sig4.red_clearance = 2000;
    sig4.concurrent_signal_groups.push_back(2);
    tsc_config->tsc_config_list.push_back(sig4);

    // TSC Configuration
    // barrier = ||
    //ring 1 : 2 || 5 6
    //ring 2 : 4 || 
    so_service.populate_movement_groups(m_groups,tsc_config);
    
    ASSERT_EQ(3, m_groups->groups.size());
    auto mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(5, mg.signal_groups.first);
    ASSERT_EQ(0, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(2, mg.signal_groups.first);
    ASSERT_EQ(4, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(6, mg.signal_groups.first);
    ASSERT_EQ(0, mg.signal_groups.second);
}

/**
 * @brief Test populate movement group method with example tsc configurations state.
 */
TEST(signal_opt_service, populate_movement_group_nema) {
    signal_opt_service::signal_opt_service so_service;
    auto tsc_config = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();
    auto m_groups = std::make_shared<streets_signal_optimization::movement_groups>();
    streets_tsc_configuration::signal_group_configuration sig1;
    sig1.signal_group_id = 1;
    sig1.yellow_change_duration = 3000;
    sig1.red_clearance = 2000;
    sig1.concurrent_signal_groups.push_back(5);
    sig1.concurrent_signal_groups.push_back(6);

    tsc_config->tsc_config_list.push_back(sig1);

    streets_tsc_configuration::signal_group_configuration sig2;
    sig2.signal_group_id = 2;
    sig2.yellow_change_duration = 3000;
    sig2.red_clearance = 2000;
    sig2.concurrent_signal_groups.push_back(5);
    sig2.concurrent_signal_groups.push_back(6);
    tsc_config->tsc_config_list.push_back(sig2);

    streets_tsc_configuration::signal_group_configuration sig3;
    sig3.signal_group_id = 3;
    sig3.yellow_change_duration = 3000;
    sig3.red_clearance = 2000;
    sig3.concurrent_signal_groups.push_back(7);
    sig3.concurrent_signal_groups.push_back(8);
    tsc_config->tsc_config_list.push_back(sig3);

    streets_tsc_configuration::signal_group_configuration sig4;
    sig4.signal_group_id = 4;
    sig4.yellow_change_duration = 3000;
    sig4.red_clearance = 2000;
    sig4.concurrent_signal_groups.push_back(7);
    sig4.concurrent_signal_groups.push_back(8);
    tsc_config->tsc_config_list.push_back(sig4);

    streets_tsc_configuration::signal_group_configuration sig5;
    sig5.signal_group_id = 5;
    sig5.yellow_change_duration = 3000;
    sig5.red_clearance = 2000;
    sig5.concurrent_signal_groups.push_back(1);
    sig5.concurrent_signal_groups.push_back(2);
    tsc_config->tsc_config_list.push_back(sig5);

    streets_tsc_configuration::signal_group_configuration sig6;
    sig6.signal_group_id = 6;
    sig6.yellow_change_duration = 3000;
    sig6.red_clearance = 2000;
    sig6.concurrent_signal_groups.push_back(1);
    sig6.concurrent_signal_groups.push_back(2);
    tsc_config->tsc_config_list.push_back(sig6);

    streets_tsc_configuration::signal_group_configuration sig7;
    sig7.signal_group_id = 7;
    sig7.yellow_change_duration = 3000;
    sig7.red_clearance = 2000;
    sig7.concurrent_signal_groups.push_back(3);
    sig7.concurrent_signal_groups.push_back(4);
    tsc_config->tsc_config_list.push_back(sig7);

    streets_tsc_configuration::signal_group_configuration sig8;
    sig8.signal_group_id = 8;
    sig8.yellow_change_duration = 3000;
    sig8.red_clearance = 2000;
    sig8.concurrent_signal_groups.push_back(3);
    sig8.concurrent_signal_groups.push_back(4);
    tsc_config->tsc_config_list.push_back(sig8);

    // TSC Configuration
    // barrier = ||
    //ring 1 : 1 2 || 3 4
    //ring 2 : 5 6 || 7 8 
    so_service.populate_movement_groups(m_groups,tsc_config);
    
    ASSERT_EQ(8, m_groups->groups.size());
    auto mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(1, mg.signal_groups.first);
    ASSERT_EQ(5, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(1, mg.signal_groups.first);
    ASSERT_EQ(6, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(2, mg.signal_groups.first);
    ASSERT_EQ(5, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(2, mg.signal_groups.first);
    ASSERT_EQ(6, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(3, mg.signal_groups.first);
    ASSERT_EQ(7, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(3, mg.signal_groups.first);
    ASSERT_EQ(8, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(4, mg.signal_groups.first);
    ASSERT_EQ(7, mg.signal_groups.second);

    mg = m_groups->groups.front();
    m_groups->groups.pop_front();

    ASSERT_EQ(4, mg.signal_groups.first);
    ASSERT_EQ(8, mg.signal_groups.second);
}