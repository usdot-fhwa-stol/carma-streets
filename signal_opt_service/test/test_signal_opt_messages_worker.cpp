#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "signal_opt_messages_worker.h"

TEST(signal_opt_messages_worker, add_update_vehicle)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    // Add vehicles
    ASSERT_EQ(0, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles().size());
    auto cur_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 1000;
    std::string vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-507\", \"v_length\": 5, \"min_gap\": 10.0, \"react_t\": 1.5, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->add_update_vehicle(vsi_payload);
    ASSERT_EQ(1, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles().size());

    cur_time += 1000;
    vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-508\", \"v_length\": 5, \"min_gap\": 10.0, \"react_t\": 1.5, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->add_update_vehicle(vsi_payload);
    ASSERT_EQ(2, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles().size());

    cur_time += 1000;
    vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-509\", \"v_length\": 5, \"min_gap\": 10.0, \"react_t\": 1.5, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->add_update_vehicle(vsi_payload);
    ASSERT_EQ(3, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles().size());

    // Update vehicles
    ASSERT_EQ(5, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles().at("DOT-507")._cur_speed / 0.02);
    ASSERT_EQ(5, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles().at("DOT-508")._cur_speed / 0.02);

    cur_time += 1000;
    vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-507\", \"v_length\": 5, \"min_gap\": 15.0, \"react_t\": 2, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 10.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->add_update_vehicle(vsi_payload);

    cur_time += 1000;
    vsi_payload = "{ \"metadata\": { \"timestamp\": " + std::to_string(cur_time) + " }, \"payload\": { \"v_id\": \"DOT-508\", \"v_length\": 5, \"min_gap\": 15.0, \"react_t\": 2, \"max_accel\": 5.0, \"max_decel\": 5.0, \"cur_speed\": 20.0, \"cur_accel\": 0.0, \"cur_lane_id\": 7, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 7, \"link_lane_id\": 8, \"dest_lane_id\": 9, \"is_allowed\": false, \"depart_pos\": 1, \"est_paths\": [{ \"ts\": 1623677096200, \"id\": 7, \"ds\": 6.0 }, { \"ts\": 1623677096400, \"id\": 7, \"ds\": 5.0 }, { \"ts\": 1623677096600, \"id\": 7, \"ds\": 4.0 }, { \"ts\": 1623677096800, \"id\": 7, \"ds\": 3.0 }, { \"ts\": 1623677097000, \"id\": 7, \"ds\": 2.0 } ] } }";
    so_msgs_worker_ptr->add_update_vehicle(vsi_payload);
    ASSERT_EQ(3, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles().size());

    ASSERT_EQ(10, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles()["DOT-507"]._cur_speed / 0.02);
    ASSERT_EQ(20, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles()["DOT-508"]._cur_speed / 0.02);
}

TEST(signal_opt_messages_worker, request_intersection_info)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    auto intersection_info_ptr = so_msgs_worker_ptr->get_intersection_info();
    ASSERT_TRUE(intersection_info_ptr != nullptr);
    ASSERT_EQ(0, intersection_info_ptr->getLinkLanelets().size());
    ASSERT_FALSE(so_msgs_worker_ptr->request_intersection_info());
}

TEST(signal_opt_messages_worker, update_spat)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    std::string spat_payload = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"revision\":123,\"status\":\"01001001001\",\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[1,3,5],\"states\":[{\"movement_name\":\"Right Turn\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":0,\"min_end_time\":0,\"max_end_time\":0,\"likely_time\":0,\"confidence\":0},\"speeds\":[{\"type\":0,\"speed_limit\":4,\"speed_confidence\":1,\"distance\":5,\"class\":5}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    ASSERT_TRUE(so_msgs_worker_ptr->update_spat(spat_payload));
    ASSERT_EQ(1909, so_msgs_worker_ptr->get_latest_spat()->intersections.front().id);
    ASSERT_EQ("West Intersection", so_msgs_worker_ptr->get_latest_spat()->intersections.front().name);
}

TEST(signal_opt_messages_worker, get_intersection_info)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    auto intersection_info_ptr = so_msgs_worker_ptr->get_intersection_info();
    ASSERT_TRUE(intersection_info_ptr != nullptr);
    ASSERT_EQ(0, intersection_info_ptr->getLinkLanelets().size());
}

TEST(signal_opt_messages_worker, get_vehicle_list)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    auto vehicle_list_ptr = so_msgs_worker_ptr->get_vehicle_list();
    ASSERT_TRUE(vehicle_list_ptr != nullptr);
    ASSERT_EQ(0, so_msgs_worker_ptr->get_vehicle_list()->get_vehicles().size());
}

TEST(signal_opt_messages_worker, get_latest_spat)
{
    auto so_msgs_worker_ptr = std::make_shared<signal_opt_service::signal_opt_messages_worker>();
    auto spat_ptr = so_msgs_worker_ptr->get_latest_spat();
    ASSERT_TRUE(spat_ptr != nullptr);
    ASSERT_EQ(0, spat_ptr->intersections.size());
}