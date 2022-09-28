#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "signal_opt_service.h"
#include "mock_kafka_consumer_worker.h"
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
    EXPECT_CALL(*mock_tsc_config_consumer,is_running()).Times(4).WillOnce(Return(false))
                                                            .WillRepeatedly(Return(true));
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
    auto m_groups = std::make_shared<signal_opt_service::movement_groups>();
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