#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "signal_opt_messages_worker.h"

TEST(signal_opt_messages_worker, update_vehicle_list)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    auto vehicle_list_ptr =  std::make_shared<streets_vehicles::vehicle_list>();
    vehicle_list_ptr->set_processor(std::make_shared<streets_vehicles::signalized_status_intent_processor>());
    // Add vehicles
    SPDLOG_INFO("Initialize! {0}");

    ASSERT_TRUE( vehicle_list_ptr->get_vehicles().empty());
    SPDLOG_INFO("Assert empty list!");
    auto cur_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 1000;
    // Valid payload
    std::string vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-507\", \"v_length\": 5, \"min_gap\": 10.0, \"react_t\": 1.5, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->update_vehicle_list(vsi_payload, vehicle_list_ptr);
    SPDLOG_INFO("Populated vehicle list");
    ASSERT_EQ(1, vehicle_list_ptr->get_vehicles().size());

    cur_time += 1000;
    vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-508\", \"v_length\": 5, \"min_gap\": 10.0, \"react_t\": 1.5, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->update_vehicle_list(vsi_payload, vehicle_list_ptr);
    ASSERT_EQ(2, vehicle_list_ptr->get_vehicles().size());

    cur_time += 1000;
    vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-509\", \"v_length\": 5, \"min_gap\": 10.0, \"react_t\": 1.5, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->update_vehicle_list(vsi_payload, vehicle_list_ptr);
    ASSERT_EQ(3, vehicle_list_ptr->get_vehicles().size());

    // Update vehicles
    ASSERT_EQ(5, vehicle_list_ptr->get_vehicles().at("DOT-507")._cur_speed / 0.02);
    ASSERT_EQ(5, vehicle_list_ptr->get_vehicles().at("DOT-508")._cur_speed / 0.02);

    cur_time += 1000;
    vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-507\", \"v_length\": 5, \"min_gap\": 15.0, \"react_t\": 2, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 10.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->update_vehicle_list(vsi_payload, vehicle_list_ptr);

    cur_time += 1000;
    vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-508\", \"v_length\": 5, \"min_gap\": 15.0, \"react_t\": 2, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 20.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->update_vehicle_list(vsi_payload, vehicle_list_ptr);
    ASSERT_EQ(3, vehicle_list_ptr->get_vehicles().size());

    ASSERT_EQ(10, vehicle_list_ptr->get_vehicles()["DOT-507"]._cur_speed / 0.02);
    ASSERT_EQ(20, vehicle_list_ptr->get_vehicles()["DOT-508"]._cur_speed / 0.02);
}

TEST(signal_opt_messages_worker, request_intersection_info)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    auto intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>();
    ASSERT_TRUE(intersection_info_ptr != nullptr);
    ASSERT_EQ(0, intersection_info_ptr->getLinkLanelets().size());
    ASSERT_FALSE(so_msgs_worker_ptr->request_intersection_info(intersection_info_ptr));
}

TEST(signal_opt_messages_worker, update_spat)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    auto spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
    // Invalid payload
    std::string spat_payload ="{"
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
        "}";
    ASSERT_FALSE(so_msgs_worker_ptr->update_spat(spat_payload, spat_ptr));
    spat_payload = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":0,\"revision\":123,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[1,3,5],\"states\":[{\"movement_name\":\"Right Turn\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":0,\"min_end_time\":0,\"max_end_time\":0,\"likely_time\":0,\"confidence\":0},\"speeds\":[{\"type\":0,\"speed_limit\":4,\"speed_confidence\":1,\"distance\":5,\"class\":5}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    ASSERT_TRUE(so_msgs_worker_ptr->update_spat(spat_payload, spat_ptr));
    ASSERT_EQ(1909, spat_ptr->get_intersection().id);
    ASSERT_EQ("West Intersection",spat_ptr->get_intersection().name);
}

TEST(signal_opt_messages_worker, update_tsc_config) {
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    auto tsc_config_ptr = std::make_shared<streets_tsc_configuration::tsc_configuration_state>();
    // Invalid payload
    std::string tsc_config_payload = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":0,\"revision\":123,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[1,3,5],\"states\":[{\"movement_name\":\"Right Turn\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":0,\"min_end_time\":0,\"max_end_time\":0,\"likely_time\":0,\"confidence\":0},\"speeds\":[{\"type\":0,\"speed_limit\":4,\"speed_confidence\":1,\"distance\":5,\"class\":5}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    ASSERT_FALSE(so_msgs_worker_ptr->update_tsc_config(tsc_config_payload, tsc_config_ptr));
    ASSERT_FALSE( tsc_config_ptr == nullptr);
    // Valid payload
    tsc_config_payload = "{"
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
        "}";

    ASSERT_TRUE( so_msgs_worker_ptr->update_tsc_config(tsc_config_payload, tsc_config_ptr));
    ASSERT_EQ( tsc_config_ptr->tsc_config_list.size(),3);
    // First Entry
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[0].signal_group_id,1);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[0].yellow_change_duration,1000);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[0].red_clearance,500);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[0].concurrent_signal_groups.size(),2);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[0].concurrent_signal_groups[0],5);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[0].concurrent_signal_groups[1],6);
    // Second Entry
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[1].signal_group_id,2);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[1].yellow_change_duration,2000);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[1].red_clearance,300);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[1].concurrent_signal_groups.size(),2);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[1].concurrent_signal_groups[0],5);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[1].concurrent_signal_groups[1],6);
    // Third Entry
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[2].signal_group_id,7);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[2].yellow_change_duration,2000);
    ASSERT_EQ( tsc_config_ptr->tsc_config_list[2].red_clearance,300);
    ASSERT_TRUE( tsc_config_ptr->tsc_config_list[2].concurrent_signal_groups.empty()); 
}
