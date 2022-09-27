#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "signal_opt_service.h"
#include "mock_kafka_consumer_worker.h"

using testing::_;
using testing::Return;


TEST(signal_opt_service, initialize)
{
    signal_opt_service::signal_opt_service so_service;
    ASSERT_FALSE(so_service.initialize());
    
}

TEST(signal_opt_service, update_intersection_info)
{
    signal_opt_service::signal_opt_service so_service;
    ASSERT_FALSE(so_service.initialize());
    ASSERT_FALSE(so_service.update_intersection_info(1000, 1));
}

TEST(signal_opt_service, consume_spat) {
    signal_opt_service::signal_opt_service so_service;
    std::shared_ptr<kafka_clients::mock_kafka_consumer_worker> mock_spat_consumer = std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
    std::shared_ptr<signal_phase_and_timing::spat> spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
    EXPECT_CALL(*mock_spat_consumer,is_running()).Times(5).WillOnce(Return(false))
                                                            .WillOnce(Return(true))
                                                            .WillOnce(Return(false))
                                                            .WillOnce(Return(true))
                                                            .WillRepeatedly(Return(false));
    EXPECT_CALL(*mock_spat_consumer, consume(1000)).Times(2).WillOnce(Return("")).WillRepeatedly(Return(
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
    // mock_spat_consumer returns false to is_running
    SPDLOG_INFO("Consuming not running!");
    so_service.consume_spat(mock_spat_consumer,spat_ptr);
    SPDLOG_INFO("Consuming Empty message");
    // mock_spat_consumer returns "" from consume(1000)
    so_service.consume_spat(mock_spat_consumer, spat_ptr);
    SPDLOG_INFO("Consuming message.");
    // mock_spat_consumer returns spat json string from consume(1000)
    so_service.consume_spat(mock_spat_consumer,spat_ptr);

}

TEST(signal_opt_service, consume_tsc_config) {
    signal_opt_service::signal_opt_service so_service;
    std::shared_ptr<kafka_clients::mock_kafka_consumer_worker> mock_tsc_config_consumer = std::make_shared<kafka_clients::mock_kafka_consumer_worker>();
    std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_config = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();
    EXPECT_CALL(*mock_tsc_config_consumer,is_running()).Times(3).WillOnce(Return(false))
                                                            .WillRepeatedly(Return(true));
    EXPECT_CALL(*mock_tsc_config_consumer, consume(1000)).Times(2).WillOnce(Return("")).WillRepeatedly(Return(
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
    // mock_spat_consumer returns false to is_running
    SPDLOG_INFO("Consuming not running!");
    so_service.consume_tsc_config(mock_tsc_config_consumer,tsc_config);
    SPDLOG_INFO("Consuming Empty message");
    // mock_spat_consumer returns "" from consume(1000)
    so_service.consume_tsc_config(mock_tsc_config_consumer, tsc_config);
   

}