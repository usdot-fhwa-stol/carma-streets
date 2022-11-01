#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "streets_desired_phase_plan_generator.h"

using namespace streets_vehicles;
using namespace streets_signal_optimization;

/**
 * @brief Test case for setting the cofiguration.
 */
TEST(test_streets_desired_phase_plan_generator, test_set_configuration) {


    streets_desired_phase_plan_generator generator;

    const uint64_t _initial_green_buffer = 1500; 
    const uint64_t _final_green_buffer = 1000;
    const uint64_t _et_inaccuracy_buffer = 2000;
    const uint64_t _queue_max_time_headway = 4000;
    const double _so_radius = 150;
    const uint64_t _min_green = 6000;
    const uint64_t _max_green = 100000;
    const uint8_t _desired_future_move_group_count = 2;

    generator.set_configuration(_initial_green_buffer, 
                                _final_green_buffer, 
                                _et_inaccuracy_buffer, 
                                _queue_max_time_headway, 
                                _so_radius, 
                                _min_green,
                                _max_green,
                                _desired_future_move_group_count);

    ASSERT_EQ( generator.get_initial_green_buffer(), 1500);
    ASSERT_EQ( generator.get_final_green_buffer(), 1000);
    ASSERT_EQ( generator.get_et_inaccuracy_buffer(), 2000);
    ASSERT_EQ( generator.get_queue_max_time_headway(), 4000);
    ASSERT_EQ( generator.get_so_radius(), 150);
    ASSERT_EQ( generator.get_min_green(), 6000);
    ASSERT_EQ( generator.get_max_green(), 100000);
    ASSERT_EQ( generator.get_desired_future_move_group_count(), 2);

    generator.set_initial_green_buffer(2500);
    generator.set_final_green_buffer(1500);
    generator.set_et_inaccuracy_buffer(1000);
    generator.set_queue_max_time_headway(2000);
    generator.set_so_radius(170);
    generator.set_min_green(7000);
    generator.set_max_green(80000);
    generator.set_desired_future_move_group_count(3);

    ASSERT_EQ( generator.get_initial_green_buffer(), 2500);
    ASSERT_EQ( generator.get_final_green_buffer(), 1500);
    ASSERT_EQ( generator.get_et_inaccuracy_buffer(), 1000);
    ASSERT_EQ( generator.get_queue_max_time_headway(), 2000);
    ASSERT_EQ( generator.get_so_radius(), 170);
    ASSERT_EQ( generator.get_min_green(), 7000);
    ASSERT_EQ( generator.get_max_green(), 80000);
    ASSERT_EQ( generator.get_desired_future_move_group_count(), 3);

}

/**
 * @brief Test case for verifying spat. 
 * In this test case, the modified spat contains the information for 4 signal groups.
 * There are two movement groups fixed in the used modified spat.
 * In the modified spat, the traffic signal is green for signal groups 1 and 3 for the next 10 seconds, yellow for 3 seconds and 
 * red-clearance for 2 seconds. Then, the traffic signal will be green for signal group 2 for 10 seconds, yellow for 3 seconds and 
 * red-clearance for 2 seconds.
 */
TEST(test_streets_desired_phase_plan_generator, test_convert_spat_to_dpp) {

    signal_phase_and_timing::spat spat_object;
    std::string json_spat = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":0,\"revision\":123,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[9, 10, 11, 12, 13, 14, 15, 16],\"states\":[{\"movement_name\":\"All Directions\",\"signal_group\":1,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}},{\"event_state\":8,\"timing\":{\"start_time\":10100,\"min_end_time\":10130}}, {\"event_state\":3,\"timing\":{\"start_time\":10130,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":2,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10150}},{\"event_state\":6,\"timing\":{\"start_time\":10150,\"min_end_time\":10250}}, {\"event_state\":8,\"timing\":{\"start_time\":10250,\"min_end_time\":10280}}, {\"event_state\":3,\"timing\":{\"start_time\":10280,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":3,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}},{\"event_state\":8,\"timing\":{\"start_time\":10100,\"min_end_time\":10130}}, {\"event_state\":3,\"timing\":{\"start_time\":10130,\"min_end_time\":10300}}]}, {\"movement_name\":\"All Directions\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10300}}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    spat_object.fromJson(json_spat);
    auto intersection_state = spat_object.get_intersection();

    /** Create a list of movement groups */
    std::shared_ptr<movement_groups> move_groups = std::make_shared<movement_groups>();
    movement_group mg1;
    mg1.name = "movement_group_1";
    mg1.signal_groups = {1, 3};
    move_groups->groups.push_back(mg1);
    movement_group mg2;
    mg2.name = "movement_group_2";
    mg2.signal_groups = {2, 0};
    move_groups->groups.push_back(mg2);
    movement_group mg3;
    mg3.name = "movement_group_3";
    mg3.signal_groups = {4, 0};
    move_groups->groups.push_back(mg3);

    streets_desired_phase_plan_generator generator;
    generator.set_configuration(2000, 2000, 2000, 3000, 200, 50000, 120000, 1);
    streets_desired_phase_plan::streets_desired_phase_plan base_desired_phase_plan = generator.convert_spat_to_dpp(intersection_state, move_groups);
    ASSERT_EQ( base_desired_phase_plan.desired_phase_plan.size(), 2);
    // SPDLOG_INFO("The number of fixed future movement groups in the modified spat: {0}", base_desired_phase_plan.desired_phase_plan.size());
    
    uint64_t _tbd_start = intersection_state.states.front().state_time_speed.back().timing.get_epoch_min_end_time();
    uint64_t tbd_start = generator.find_tbd_start_time(intersection_state);
    ASSERT_EQ( tbd_start, _tbd_start);
    // SPDLOG_INFO("TBD start time: {0}", tbd_start);
    
    uint64_t first_mg_start_time = intersection_state.states.front().state_time_speed.front().timing.get_epoch_start_time();
    uint64_t first_mg_end_time = intersection_state.states.front().state_time_speed.front().timing.get_epoch_min_end_time();
    // SPDLOG_INFO("First movement group in the base desired phase plan - start time: {0}", base_desired_phase_plan.desired_phase_plan.front().start_time);
    ASSERT_EQ( base_desired_phase_plan.desired_phase_plan.front().start_time, first_mg_start_time);
    // SPDLOG_INFO("First movement group in the base desired phase plan - end time: {0}", base_desired_phase_plan.desired_phase_plan.front().end_time);
    ASSERT_EQ( base_desired_phase_plan.desired_phase_plan.front().end_time, first_mg_end_time);
    // SPDLOG_INFO("First movement group in the base desired phase plan - number of signal groups: {0}", base_desired_phase_plan.desired_phase_plan.front().signal_groups.size());
    ASSERT_EQ( base_desired_phase_plan.desired_phase_plan.front().signal_groups.size(), 2);
    // SPDLOG_INFO("Second movement group in the base desired phase plan - start time: {0}", base_desired_phase_plan.desired_phase_plan.back().start_time);
    ASSERT_EQ( base_desired_phase_plan.desired_phase_plan.back().start_time, first_mg_end_time + 5000);
    // SPDLOG_INFO("Second movement group in the base desired phase plan - end time: {0}", base_desired_phase_plan.desired_phase_plan.back().end_time);
    ASSERT_EQ( base_desired_phase_plan.desired_phase_plan.back().end_time, first_mg_end_time + 5000 + 10000);
    // SPDLOG_INFO("Second movement group in the base desired phase plan - number of signal groups: {0}", base_desired_phase_plan.desired_phase_plan.back().signal_groups.size());
    ASSERT_EQ( base_desired_phase_plan.desired_phase_plan.back().signal_groups.size(), 1);


    /** 
     * Failure case: partially overlapping greens. 
     * case 1: two signal groups from different movement groups have partial overlapping greens.
     */
    std::string json_spat_fail = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":0,\"revision\":123,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[9, 10, 11, 12, 13, 14, 15, 16],\"states\":[{\"movement_name\":\"All Directions\",\"signal_group\":1,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}},{\"event_state\":8,\"timing\":{\"start_time\":10100,\"min_end_time\":10130}}, {\"event_state\":3,\"timing\":{\"start_time\":10130,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":2,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10080}},{\"event_state\":6,\"timing\":{\"start_time\":10080,\"min_end_time\":10250}}, {\"event_state\":8,\"timing\":{\"start_time\":10250,\"min_end_time\":10280}}, {\"event_state\":3,\"timing\":{\"start_time\":10280,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":3,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}},{\"event_state\":8,\"timing\":{\"start_time\":10100,\"min_end_time\":10130}}, {\"event_state\":3,\"timing\":{\"start_time\":10130,\"min_end_time\":10300}}]}, {\"movement_name\":\"All Directions\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10300}}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    spat_object.fromJson(json_spat_fail);
    intersection_state = spat_object.get_intersection();
    ASSERT_THROW(generator.convert_spat_to_dpp(intersection_state, move_groups), streets_desired_phase_plan_generator_exception);

    /** 
     * Failure case: partially overlapping greens. 
     * case 1: two signal groups from the same movement groups have partial overlapping greens.
     */
    json_spat_fail = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":0,\"revision\":123,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[9, 10, 11, 12, 13, 14, 15, 16],\"states\":[{\"movement_name\":\"All Directions\",\"signal_group\":1,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}},{\"event_state\":8,\"timing\":{\"start_time\":10100,\"min_end_time\":10130}}, {\"event_state\":3,\"timing\":{\"start_time\":10130,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":2,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10150}},{\"event_state\":6,\"timing\":{\"start_time\":10150,\"min_end_time\":10250}}, {\"event_state\":8,\"timing\":{\"start_time\":10250,\"min_end_time\":10280}}, {\"event_state\":3,\"timing\":{\"start_time\":10280,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":3,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":9950,\"min_end_time\":10120}},{\"event_state\":8,\"timing\":{\"start_time\":10120,\"min_end_time\":10140}}, {\"event_state\":3,\"timing\":{\"start_time\":10140,\"min_end_time\":10300}}]}, {\"movement_name\":\"All Directions\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10300}}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    spat_object.fromJson(json_spat_fail);
    intersection_state = spat_object.get_intersection();

    ASSERT_THROW(generator.convert_spat_to_dpp(intersection_state, move_groups), streets_desired_phase_plan_generator_exception);

}



/**
 * @brief Test case for finding the entry_lane_signal_group_mapping.
 * The intersection_info used in this test contains 5 entry lanes, 5 departure lanes, and 10 connection links.
 * All connection links connected to entry lanes 1 and 17 are all connected to signal group 1.
 * All connection links connected to entry lane 2 are all connected to signal group 2.
 * All connection links connected to entry lane 3 are all connected to signal group 3.
 * All connection links connected to entry lane 4 are all connected to signal group 4.
 */
TEST(test_streets_desired_phase_plan_generator, test_signal_group_entry_lane_mapping) {

    streets_desired_phase_plan_generator generator;
    generator.set_configuration(2000, 2000, 2000, 3000, 250, 50000, 120000, 3);

    OpenAPI::OAIIntersection_info info;
    std::string json_info = "{\"departure_lanelets\":[{ \"id\":5, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":6, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":7, \"length\":34.130869420842046, \"speed_limit\":11.176 }, { \"id\":8, \"length\":50.123213235343123, \"speed_limit\":11.176 }, { \"id\":18, \"length\":50.123213235343123, \"speed_limit\":11.176 }], \"entry_lanelets\":[{ \"id\":1, \"length\":195.73023157287864, \"speed_limit\":11.176, \"connecting_lanelet_ids\": [9, 10] }, { \"id\":2, \"length\":34.130869411176431136, \"speed_limit\":11.176, \"connecting_lanelet_ids\": [11, 12] }, { \"id\":3, \"length\":41.60952435603712, \"speed_limit\":11.176 , \"connecting_lanelet_ids\": [13, 14]}, { \"id\":4, \"length\":53.19846216254821, \"speed_limit\":11.176 , \"connecting_lanelet_ids\": [15, 16]}, { \"id\":17, \"length\":53.19846216254821, \"speed_limit\":11.176 , \"connecting_lanelet_ids\": [19, 20]}], \"id\":9001, \"link_lanelets\":[{ \"conflict_lanelet_ids\":[ 11, 15, 16 ], \"id\":9, \"length\":15.85409574709938, \"speed_limit\":11.176, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 11 ], \"id\":10, \"length\":16.796388658952235, \"speed_limit\":4.4704, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 9, 10, 13 ], \"id\":11, \"length\":16.043077028554038, \"speed_limit\":11.176, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[ 13 ], \"id\":12, \"length\":10.295559117055083, \"speed_limit\":4.4704, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[ 11, 12, 15 ], \"id\":13, \"length\":15.853947840111768943, \"speed_limit\":11.176, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 15 ], \"id\":14, \"length\":9.744590320260139, \"speed_limit\":4.4704, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 9, 13, 14 ], \"id\":15, \"length\":13.6473819283719203846, \"speed_limit\":11.176, \"signal_group_id\":4 }, { \"conflict_lanelet_ids\":[ 9 ], \"id\":16, \"length\":8.182736100981263, \"speed_limit\":4.4704, \"signal_group_id\":4 }, { \"conflict_lanelet_ids\":[ 11, 15, 16 ], \"id\":19, \"length\":14.12329574709938, \"speed_limit\":11.176, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 11 ], \"id\":20, \"length\":14.123388658952235, \"speed_limit\":4.4704, \"signal_group_id\":1 }], \"name\":\"WestIntersection\"}";  
    info.fromJson(QString::fromStdString(json_info));
    std::shared_ptr<OpenAPI::OAIIntersection_info> intersection = std::make_shared<OpenAPI::OAIIntersection_info>(info);

    generator.create_signal_group_entry_lane_mapping(intersection);
    std::unordered_map<uint8_t, std::vector<int>> signal_group_entry_lane_mapping = generator.get_signal_group_entry_lane_mapping();
    ASSERT_EQ(signal_group_entry_lane_mapping.size(), 4);
    // SPDLOG_INFO("Signal group to entry lane mapping - signal group {0}: [{1}, {2}]", 1, signal_group_entry_lane_mapping.at(1)[0], signal_group_entry_lane_mapping.at(1)[1]);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(1).size(), 2);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(1)[0], 1);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(1)[1], 17);
    // SPDLOG_INFO("Signal group to entry lane mapping - signal group {0}: {1}", 2, signal_group_entry_lane_mapping.at(2)[0]);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(2).size(), 1);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(2)[0], 2);
    // SPDLOG_INFO("Signal group to entry lane mapping - signal group {0}: {1}", 3, signal_group_entry_lane_mapping.at(4)[0]);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(3).size(), 1);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(3)[0], 3);
    // SPDLOG_INFO("Signal group to entry lane mapping - signal group {0}: {1}", 4, signal_group_entry_lane_mapping.at(4)[0]);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(4).size(), 1);
    ASSERT_EQ(signal_group_entry_lane_mapping.at(4)[0], 4);
    
}



/**
 * @brief Test case for generating desired_phase_plan_list.
 * In this test case, the modified spat contains the information for 4 signal groups.
 * There is one movement group fixed in the used modified spat.
 * In the modified spat, the traffic signal will be green for signal group 2 for 10 seconds, yellow for 3 seconds and 
 * red-clearance for 2 seconds.
 * 
 * The intersection_info used in this test contains 4 entry lanes, 4 departure lanes, and 8 connection links.
 * All connection links connected to entry lane 1 are all connected to signal group 1.
 * All connection links connected to entry lane 2 are all connected to signal group 2.
 * All connection links connected to entry lane 3 are all connected to signal group 3.
 * All connection links connected to entry lane 4 are all connected to signal group 4.
 * 
 * In this test case, 13 vehicles are considered. 1 DV and 12 EVs.
 * The first 6 EVs (TEST_EV_01 to TEST_EV_06) are located in entry lane 1 which is connected to signal group 1.
 * The second 3 EVs (TEST_EV_07 to TEST_EV_09) are located in entry lane 4 which is connected to signal group 4.
 * The third 3 EVs (TEST_EV_10 to TEST_EV_12) are located in entry lane 3 which is connected to signal group 3.
 * 
 * In this test case, 3 possible movement groups are considered:
 * - Movement group 1 : {signal group 1, signal group 3}
 * - Movement group 2 : {signal group 2}
 * - Movement group 3 : {signal group 4}
 * 
 * Finally, the desired phase plan list shall include shall include 3 desired phase plans:
 * - desired_phase_plan 1: [{movement group 2}, {movement_group 1}] with green duration calculated for entry lane 1
 * - desired_phase_plan 2: [{movement group 2}, {movement_group 1}] with green duration calculated for entry lane 3
 * - desired_phase_plan 3: [{movement group 2}, {movement_group 3}] with green duration calculated for entry lane 4
 */
namespace streets_signal_optimization {

TEST(test_streets_desired_phase_plan_generator, test_generate_desired_phase_plan_list) {

    streets_desired_phase_plan_generator generator;
    generator.set_configuration(2000, 2000, 2000, 3000, 250, 50000, 120000, 3);

    OpenAPI::OAIIntersection_info info;
    std::string json_info = "{\"departure_lanelets\":[{ \"id\":5, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":6, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":7, \"length\":34.130869420842046, \"speed_limit\":11.176 }, { \"id\":8, \"length\":50.123213235343123, \"speed_limit\":11.176 }], \"entry_lanelets\":[{ \"id\":1, \"length\":195.73023157287864, \"speed_limit\":8.0, \"connecting_lanelet_ids\": [9, 10] }, { \"id\":2, \"length\":34.130869411176431136, \"speed_limit\":8.0, \"connecting_lanelet_ids\": [11, 12] }, { \"id\":3, \"length\":41.60952435603712, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [13, 14]}, { \"id\":4, \"length\":53.19846216254821, \"speed_limit\":8.0 , \"connecting_lanelet_ids\": [15, 16]}], \"id\":9001, \"link_lanelets\":[{ \"conflict_lanelet_ids\":[ 11, 15, 16 ], \"id\":9, \"length\":15.85409574709938, \"speed_limit\":8.0, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 11 ], \"id\":10, \"length\":16.796388658952235, \"speed_limit\":4.4704, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 9, 10, 13 ], \"id\":11, \"length\":16.043077028554038, \"speed_limit\":8.0, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[ 13 ], \"id\":12, \"length\":10.295559117055083, \"speed_limit\":4.4704, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[ 11, 12, 15 ], \"id\":13, \"length\":15.853947840111768943, \"speed_limit\":8.0, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 15 ], \"id\":14, \"length\":9.744590320260139, \"speed_limit\":4.4704, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 9, 13, 14 ], \"id\":15, \"length\":13.6473819283719203846, \"speed_limit\":8.0, \"signal_group_id\":4 }, { \"conflict_lanelet_ids\":[ 9 ], \"id\":16, \"length\":8.182736100981263, \"speed_limit\":4.4704, \"signal_group_id\":4 }], \"name\":\"WestIntersection\"}";  
    info.fromJson(QString::fromStdString(json_info));
    std::shared_ptr<OpenAPI::OAIIntersection_info> intersection = std::make_shared<OpenAPI::OAIIntersection_info>(info);

    signal_phase_and_timing::spat spat_object;
    std::string json_spat = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":0,\"revision\":123,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[9, 10, 11, 12, 13, 14, 15, 16],\"states\":[{\"movement_name\":\"All Directions\",\"signal_group\":1,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}}]},{\"movement_name\":\"All Directions\",\"signal_group\":2,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10000}},{\"event_state\":6,\"timing\":{\"start_time\":10000,\"min_end_time\":10050}}, {\"event_state\":8,\"timing\":{\"start_time\":10050,\"min_end_time\":10080}}, {\"event_state\":3,\"timing\":{\"start_time\":10080,\"min_end_time\":10100}}]},{\"movement_name\":\"All Directions\",\"signal_group\":3,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}}]}, {\"movement_name\":\"All Directions\",\"signal_group\":4,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
    spat_object.fromJson(json_spat);
    auto intersection_state = spat_object.get_intersection();


    uint64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

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
    for (auto &state : intersection_state.states) {
        d2_iterator = 0;
        for (auto &state_timing : state.state_time_speed) {
            state_timing.timing.set_start_time(spat_timing[d1_iterator][d2_iterator].first);
            state_timing.timing.set_min_end_time(spat_timing[d1_iterator][d2_iterator].second);
            d2_iterator += 1;
        }
        d1_iterator += 1;
    }

    /** Create a list of movement groups */
    std::shared_ptr<movement_groups> move_groups;
    movement_group mg1;
    mg1.name = "movement_group 1";
    mg1.signal_groups = {1, 3};
    move_groups->groups.push_back(mg1);
    movement_group mg2;
    mg2.name = "movement_group 2";
    mg2.signal_groups = {2, 0};
    move_groups->groups.push_back(mg2);
    movement_group mg3;
    mg3.name = "movement_group 3";
    mg3.signal_groups = {4, 0};
    move_groups->groups.push_back(mg3);

    /** Add vehicle updates */
    vehicle veh_dv;
    veh_dv._id = "TEST_DV_01";
    veh_dv._length = 5.0;
    veh_dv._min_gap = 5.0;
    veh_dv._reaction_time = 0.5;
    veh_dv._accel_max = 1.0;
    veh_dv._decel_max = -1.0;
    veh_dv._cur_speed = 6.0;
    veh_dv._cur_accel = 0.0;
    veh_dv._cur_distance = 6.0;
    veh_dv._cur_lane_id = 9;
    veh_dv._cur_state = vehicle_state::DV;
    veh_dv._cur_time = current_time;
    veh_dv._entry_lane_id = 1;
    veh_dv._link_id = 9;
    veh_dv._exit_lane_id = 5;
    veh_dv._direction = "right";
    veh_dv._actual_et = current_time - 2000;

    vehicle veh_ev1;
    veh_ev1._id = "TEST_EV_01";
    veh_ev1._length = 5.0;
    veh_ev1._min_gap = 5.0;
    veh_ev1._reaction_time = 0.5;
    veh_ev1._accel_max = 1.0;
    veh_ev1._decel_max = -1.0;
    veh_ev1._cur_speed = 6.0;
    veh_ev1._cur_accel = 0.0;
    veh_ev1._cur_distance = 50.0;
    veh_ev1._cur_lane_id = 1;
    veh_ev1._cur_state = vehicle_state::EV;
    veh_ev1._cur_time = current_time;
    veh_ev1._entry_lane_id = 1;
    veh_ev1._link_id = 9;
    veh_ev1._exit_lane_id = 5;
    veh_ev1._direction = "straight";

    vehicle veh_ev2;
    veh_ev2._id = "TEST_EV_02";
    veh_ev2._length = 5.0;
    veh_ev2._min_gap = 5.0;
    veh_ev2._reaction_time = 0.5;
    veh_ev2._accel_max = 1.0;
    veh_ev2._decel_max = -1.0;
    veh_ev2._cur_speed = 6.0;
    veh_ev2._cur_accel = 0.0;
    veh_ev2._cur_distance = 70.0;
    veh_ev2._cur_lane_id = 1;
    veh_ev2._cur_state = vehicle_state::EV;
    veh_ev2._cur_time = current_time;
    veh_ev2._entry_lane_id = 1;
    veh_ev2._link_id = 9;
    veh_ev2._exit_lane_id = 5;
    veh_ev2._direction = "straight";

    vehicle veh_ev3;
    veh_ev3._id = "TEST_EV_03";
    veh_ev3._length = 5.0;
    veh_ev3._min_gap = 5.0;
    veh_ev3._reaction_time = 0.5;
    veh_ev3._accel_max = 1.0;
    veh_ev3._decel_max = -1.0;
    veh_ev3._cur_speed = 6.0;
    veh_ev3._cur_accel = 0.0;
    veh_ev3._cur_distance = 90.0;
    veh_ev3._cur_lane_id = 1;
    veh_ev3._cur_state = vehicle_state::EV;
    veh_ev3._cur_time = current_time;
    veh_ev3._entry_lane_id = 1;
    veh_ev3._link_id = 9;
    veh_ev3._exit_lane_id = 5;
    veh_ev3._direction = "straight";

    vehicle veh_ev4;
    veh_ev4._id = "TEST_EV_04";
    veh_ev4._length = 5.0;
    veh_ev4._min_gap = 5.0;
    veh_ev4._reaction_time = 0.5;
    veh_ev4._accel_max = 1.0;
    veh_ev4._decel_max = -1.0;
    veh_ev4._cur_speed = 6.0;
    veh_ev4._cur_accel = 0.0;
    veh_ev4._cur_distance = 130.0;
    veh_ev4._cur_lane_id = 1;
    veh_ev4._cur_state = vehicle_state::EV;
    veh_ev4._cur_time = current_time;
    veh_ev4._entry_lane_id = 1;
    veh_ev4._link_id = 9;
    veh_ev4._exit_lane_id = 5;
    veh_ev4._direction = "straight";

    vehicle veh_ev5;
    veh_ev5._id = "TEST_EV_05";
    veh_ev5._length = 5.0;
    veh_ev5._min_gap = 5.0;
    veh_ev5._reaction_time = 0.5;
    veh_ev5._accel_max = 1.0;
    veh_ev5._decel_max = -1.0;
    veh_ev5._cur_speed = 6.0;
    veh_ev5._cur_accel = 0.0;
    veh_ev5._cur_distance = 170.0;
    veh_ev5._cur_lane_id = 1;
    veh_ev5._cur_state = vehicle_state::EV;
    veh_ev5._cur_time = current_time;
    veh_ev5._entry_lane_id = 1;
    veh_ev5._link_id = 9;
    veh_ev5._exit_lane_id = 5;
    veh_ev5._direction = "straight";

    vehicle veh_ev6;
    veh_ev6._id = "TEST_EV_06";
    veh_ev6._length = 5.0;
    veh_ev6._min_gap = 5.0;
    veh_ev6._reaction_time = 0.5;
    veh_ev6._accel_max = 1.0;
    veh_ev6._decel_max = -1.0;
    veh_ev6._cur_speed = 6.0;
    veh_ev6._cur_accel = 0.0;
    veh_ev6._cur_distance = 240.0;
    veh_ev6._cur_lane_id = 1;
    veh_ev6._cur_state = vehicle_state::EV;
    veh_ev6._cur_time = current_time;
    veh_ev6._entry_lane_id = 1;
    veh_ev6._link_id = 9;
    veh_ev6._exit_lane_id = 5;
    veh_ev6._direction = "straight";

    vehicle veh_ev7;
    veh_ev7._id = "TEST_EV_07";
    veh_ev7._length = 5.0;
    veh_ev7._min_gap = 5.0;
    veh_ev7._reaction_time = 0.5;
    veh_ev7._accel_max = 1.0;
    veh_ev7._decel_max = -1.0;
    veh_ev7._cur_speed = 5.0;
    veh_ev7._cur_accel = 0.0;
    veh_ev7._cur_distance = 70.0;
    veh_ev7._cur_lane_id = 4;
    veh_ev7._cur_state = vehicle_state::EV;
    veh_ev7._cur_time = current_time;
    veh_ev7._entry_lane_id = 4;
    veh_ev7._link_id = 15;
    veh_ev7._exit_lane_id = 8;
    veh_ev7._direction = "straight";

    vehicle veh_ev8;
    veh_ev8._id = "TEST_EV_08";
    veh_ev8._length = 5.0;
    veh_ev8._min_gap = 5.0;
    veh_ev8._reaction_time = 0.5;
    veh_ev8._accel_max = 1.0;
    veh_ev8._decel_max = -1.0;
    veh_ev8._cur_speed = 5.0;
    veh_ev8._cur_accel = 0.0;
    veh_ev8._cur_distance = 100.0;
    veh_ev8._cur_lane_id = 4;
    veh_ev8._cur_state = vehicle_state::EV;
    veh_ev8._cur_time = current_time;
    veh_ev8._entry_lane_id = 4;
    veh_ev8._link_id = 15;
    veh_ev8._exit_lane_id = 8;
    veh_ev8._direction = "straight";

    vehicle veh_ev9;
    veh_ev9._id = "TEST_EV_09";
    veh_ev9._length = 5.0;
    veh_ev9._min_gap = 5.0;
    veh_ev9._reaction_time = 0.5;
    veh_ev9._accel_max = 1.0;
    veh_ev9._decel_max = -1.0;
    veh_ev9._cur_speed = 5.0;
    veh_ev9._cur_accel = 0.0;
    veh_ev9._cur_distance = 150.0;
    veh_ev9._cur_lane_id = 4;
    veh_ev9._cur_state = vehicle_state::EV;
    veh_ev9._cur_time = current_time;
    veh_ev9._entry_lane_id = 4;
    veh_ev9._link_id = 15;
    veh_ev9._exit_lane_id = 8;
    veh_ev9._direction = "straight";

    vehicle veh_ev10;
    veh_ev10._id = "TEST_EV_10";
    veh_ev10._length = 5.0;
    veh_ev10._min_gap = 5.0;
    veh_ev10._reaction_time = 0.5;
    veh_ev10._accel_max = 1.0;
    veh_ev10._decel_max = -1.0;
    veh_ev10._cur_speed = 2.0;
    veh_ev10._cur_accel = 0.0;
    veh_ev10._cur_distance = 160.0;
    veh_ev10._cur_lane_id = 3;
    veh_ev10._cur_state = vehicle_state::EV;
    veh_ev10._cur_time = current_time;
    veh_ev10._entry_lane_id = 3;
    veh_ev10._link_id = 13;
    veh_ev10._exit_lane_id = 7;
    veh_ev10._direction = "straight";

    vehicle veh_ev11;
    veh_ev11._id = "TEST_EV_11";
    veh_ev11._length = 5.0;
    veh_ev11._min_gap = 5.0;
    veh_ev11._reaction_time = 0.5;
    veh_ev11._accel_max = 1.0;
    veh_ev11._decel_max = -1.0;
    veh_ev11._cur_speed = 2.0;
    veh_ev11._cur_accel = 0.0;
    veh_ev11._cur_distance = 200.0;
    veh_ev11._cur_lane_id = 3;
    veh_ev11._cur_state = vehicle_state::EV;
    veh_ev11._cur_time = current_time;
    veh_ev11._entry_lane_id = 3;
    veh_ev11._link_id = 13;
    veh_ev11._exit_lane_id = 7;
    veh_ev11._direction = "straight";

    vehicle veh_ev12;
    veh_ev12._id = "TEST_EV_12";
    veh_ev12._length = 5.0;
    veh_ev12._min_gap = 5.0;
    veh_ev12._reaction_time = 0.5;
    veh_ev12._accel_max = 1.0;
    veh_ev12._decel_max = -1.0;
    veh_ev12._cur_speed = 2.0;
    veh_ev12._cur_accel = 0.0;
    veh_ev12._cur_distance = 240.0;
    veh_ev12._cur_lane_id = 3;
    veh_ev12._cur_state = vehicle_state::EV;
    veh_ev12._cur_time = current_time;
    veh_ev12._entry_lane_id = 3;
    veh_ev12._link_id = 13;
    veh_ev12._exit_lane_id = 7;
    veh_ev12._direction = "straight";

    std::unordered_map<std::string, vehicle> veh_list;
    veh_list.insert({{veh_dv._id, veh_dv}, {veh_ev1._id, veh_ev1}, {veh_ev2._id, veh_ev2}, {veh_ev3._id, veh_ev3}, {veh_ev4._id, veh_ev4}, {veh_ev5._id, veh_ev5}, {veh_ev6._id, veh_ev6}, {veh_ev7._id, veh_ev7}, {veh_ev8._id, veh_ev8}, {veh_ev9._id, veh_ev9}, {veh_ev10._id, veh_ev10}, {veh_ev11._id, veh_ev11}, {veh_ev12._id, veh_ev12}});
    

    /** Generate the list of desired phase plans. */
    generator.configure_scheduler(intersection);
    std::vector<streets_desired_phase_plan::streets_desired_phase_plan> desired_phase_plan_list = generator.generate_desire_phase_plan_list(intersection, veh_list, intersection_state, move_groups);
    uint64_t tbd_start_time = generator.find_tbd_start_time(intersection_state);
    ASSERT_LE( tbd_start_time, current_time + 10000 + 200);
    SPDLOG_INFO("Current timestamp: {0}, TBD start time: {1}", current_time, tbd_start_time);


    /** Seperate the vehicle schedules base on their entry lanes */
    std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> sched_ptr = generator.get_schedule_plan(veh_list);
    auto schedule_ptr = std::dynamic_pointer_cast<streets_vehicle_scheduler::signalized_intersection_schedule> (sched_ptr);
    ASSERT_EQ( schedule_ptr->vehicle_schedules.size(), 13);
    std::unordered_map<int, std::vector<streets_vehicle_scheduler::signalized_vehicle_schedule>> entry_lane_to_vehicle_sched_mapping;
    entry_lane_to_vehicle_sched_mapping.try_emplace(1, std::vector<streets_vehicle_scheduler::signalized_vehicle_schedule>());
    entry_lane_to_vehicle_sched_mapping.try_emplace(3, std::vector<streets_vehicle_scheduler::signalized_vehicle_schedule>());
    entry_lane_to_vehicle_sched_mapping.try_emplace(4, std::vector<streets_vehicle_scheduler::signalized_vehicle_schedule>());
    for (auto veh_sched : schedule_ptr->vehicle_schedules){
        if (veh_sched.entry_lane == 1) {
            entry_lane_to_vehicle_sched_mapping.at(1).push_back(veh_sched);
        }
        else if (veh_sched.entry_lane == 3){
            entry_lane_to_vehicle_sched_mapping.at(3).push_back(veh_sched);
        }
        else if (veh_sched.entry_lane == 4){
            entry_lane_to_vehicle_sched_mapping.at(4).push_back(veh_sched);
        }
    }

    /** Calculate the end time of the required green for each entry lanes based on the vehicle schedules.
     *  Find the last vehicle in the queue first, then calculate the end of the green based on the implemented algorithms.
     */
    std::unordered_map<int, uint64_t> entry_lane_to_green_end_mapping;
    streets_vehicle_scheduler::signalized_vehicle_schedule prev_veh_sched;
    uint64_t green_end;
    int vehicle_in_tbd_count;
    int vehicle_in_queue_count;
    bool is_last_vehicle_in_queue_found;
    for (const auto &[entry_lane_id, veh_sched_list] : entry_lane_to_vehicle_sched_mapping) {
        is_last_vehicle_in_queue_found = false;
        vehicle_in_tbd_count = 0;
        // SPDLOG_INFO("Estimated ET for vehicles from entry lane {0} :", entry_lane_id);
        for (const auto &veh_sched : veh_sched_list) {
            // SPDLOG_INFO("Schedule for vehicle {0} from entry lane {1} -> ET = {2}, EET = {3} :", veh_sched.v_id, entry_lane_id, veh_sched.et, veh_sched.eet);
            if (veh_sched.et > tbd_start_time) {
                vehicle_in_tbd_count += 1;
            }
        }
        vehicle_in_queue_count = 0;
        for (const auto &veh_sched : veh_sched_list) {
            if (veh_sched.et > tbd_start_time) {
                if (veh_sched.v_id != veh_sched_list.front().v_id && 
                                    veh_sched.et == veh_sched.eet && 
                                    (veh_sched.et - prev_veh_sched.et >= generator.get_queue_max_time_headway())) {
                    green_end = std::min(std::max(uint64_t(std::ceil(double(prev_veh_sched.et)/1000.0) * 1000) + generator.get_et_inaccuracy_buffer() + generator.get_final_green_buffer(), tbd_start_time + generator.get_min_green()), tbd_start_time + generator.get_max_green());
                    is_last_vehicle_in_queue_found = true;
                    break;
                }
                vehicle_in_queue_count += 1;
            }
            prev_veh_sched = veh_sched;
        }
        if (!is_last_vehicle_in_queue_found && veh_sched_list.back().et > tbd_start_time) {
            green_end = std::min(std::max(uint64_t(std::ceil(double(veh_sched_list.back().et)/1000.0) * 1000) + generator.get_et_inaccuracy_buffer() + generator.get_final_green_buffer(), tbd_start_time + generator.get_min_green()), tbd_start_time + generator.get_max_green());
            is_last_vehicle_in_queue_found = true;
        }
        // SPDLOG_INFO("Number of vehicles from entry lane {0} with an ET after tbd start time = {1}", entry_lane_id, vehicle_in_tbd_count);
        // SPDLOG_INFO("Number of vehicles from entry lane {0} that are in the queue = {1}", entry_lane_id, vehicle_in_queue_count);
        if (is_last_vehicle_in_queue_found) {
            entry_lane_to_green_end_mapping.try_emplace(entry_lane_id, green_end);
            // SPDLOG_INFO("The end of the required green for entry lane {0}: {1}", entry_lane_id, green_end);
        }
    }

    ASSERT_EQ( desired_phase_plan_list.size(), entry_lane_to_green_end_mapping.size());

    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan.size(), 2);
    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[0].start_time, tbd_start_time - 10000);
    //SPDLOG_INFO("First candidate desired_phase_plan - First movement group - start_time: {0}", desired_phase_plan_list[0].desired_phase_plan[0].start_time);
    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[0].end_time, tbd_start_time - 5000);
    //SPDLOG_INFO("First candidate desired_phase_plan - First movement group - end_time: {0}", desired_phase_plan_list[0].desired_phase_plan[0].end_time);
    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[0].signal_groups.size(), 1);
    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[0].signal_groups[0], 2);
    //SPDLOG_INFO("First candidate desired_phase_plan - First movement group - signal group ids: [{0}]", desired_phase_plan_list[0].desired_phase_plan[0].signal_groups[0]);

    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[1].start_time, tbd_start_time);
    //SPDLOG_INFO("First candidate desired_phase_plan - Second movement group - start_time: {0}", desired_phase_plan_list[0].desired_phase_plan[1].start_time);
    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[1].end_time, entry_lane_to_green_end_mapping.at(1));
    //SPDLOG_INFO("First candidate desired_phase_plan - Second movement group - end_time: {0}", desired_phase_plan_list[0].desired_phase_plan[1].end_time);
    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[1].signal_groups.size(), 2);
    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[1].signal_groups[0], 1);
    ASSERT_EQ( desired_phase_plan_list[0].desired_phase_plan[1].signal_groups[1], 3);
    //SPDLOG_INFO("First candidate desired_phase_plan - Second movement group - signal group ids: [{0}, {1}]", desired_phase_plan_list[0].desired_phase_plan[1].signal_groups[0], desired_phase_plan_list[0].desired_phase_plan[1].signal_groups[1]);

    
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan.size(), 2);
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[0].start_time, tbd_start_time - 10000);
    //SPDLOG_INFO("Second candidate desired_phase_plan - First movement group - start_time: {0}", desired_phase_plan_list[1].desired_phase_plan[0].start_time);
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[0].end_time, tbd_start_time - 5000);
    //SPDLOG_INFO("Second candidate desired_phase_plan - First movement group - end_time: {0}", desired_phase_plan_list[1].desired_phase_plan[0].end_time);
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[0].signal_groups.size(), 1);
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[0].signal_groups[0], 2);
    //SPDLOG_INFO("Second candidate desired_phase_plan - First movement group - signal group ids: [{0}]", desired_phase_plan_list[1].desired_phase_plan[0].signal_groups[0]);

    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[1].start_time, tbd_start_time);
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[1].end_time, entry_lane_to_green_end_mapping.at(3));
    //SPDLOG_INFO("Second candidate desired_phase_plan - Second movement group - start_time: {0}", desired_phase_plan_list[1].desired_phase_plan[1].start_time);
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[1].signal_groups.size(), 2);
    //SPDLOG_INFO("Second candidate desired_phase_plan - Second movement group - end_time: {0}", desired_phase_plan_list[1].desired_phase_plan[1].end_time);
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[1].signal_groups[0], 1);
    ASSERT_EQ( desired_phase_plan_list[1].desired_phase_plan[1].signal_groups[1], 3);
    //SPDLOG_INFO("Second candidate desired_phase_plan - Second movement group - signal group ids: [{0}, {1}]", desired_phase_plan_list[1].desired_phase_plan[1].signal_groups[0], desired_phase_plan_list[1].desired_phase_plan[1].signal_groups[1]);


    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan.size(), 2);
    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan[0].start_time, tbd_start_time - 10000);
    //SPDLOG_INFO("Third candidate desired_phase_plan - First movement group - start_time: {0}", desired_phase_plan_list[2].desired_phase_plan[0].start_time);
    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan[0].end_time, tbd_start_time - 5000);
    //SPDLOG_INFO("Third candidate desired_phase_plan - First movement group - end_time: {0}", desired_phase_plan_list[2].desired_phase_plan[0].end_time);
    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan[0].signal_groups.size(), 1);
    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan[0].signal_groups[0], 2);
    //SPDLOG_INFO("Third candidate desired_phase_plan - First movement group - signal group ids: [{0}]", desired_phase_plan_list[2].desired_phase_plan[0].signal_groups[0]);

    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan[1].start_time, tbd_start_time);
    //SPDLOG_INFO("Third candidate desired_phase_plan - Second movement group - start_time: {0}", desired_phase_plan_list[2].desired_phase_plan[1].start_time);
    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan[1].end_time, entry_lane_to_green_end_mapping.at(4));
    //SPDLOG_INFO("Third candidate desired_phase_plan - Second movement group - end_time: {0}", desired_phase_plan_list[2].desired_phase_plan[1].end_time);
    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan[1].signal_groups.size(), 1);
    ASSERT_EQ( desired_phase_plan_list[2].desired_phase_plan[1].signal_groups[0], 4);
    //SPDLOG_INFO("Third candidate desired_phase_plan - Second movement group - signal group ids: [{0}]", desired_phase_plan_list[2].desired_phase_plan[1].signal_groups[0]);

}
}