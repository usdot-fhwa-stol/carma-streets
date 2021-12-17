#include <rapidjson/document.h>

#include "gtest/gtest.h"
#include "message_lanelet2_translation.h"

TEST(test_message_lanelet2_translation, read_lanelet2_map)
{
    message_services::message_translations::message_lanelet2_translation clt;
    ASSERT_TRUE(clt.read_lanelet2_map("../vector_map.osm"));
    ASSERT_FALSE(clt.read_lanelet2_map("../fake_map_path.osm"));
}

TEST(test_message_lanelet2_translation, update_vehicle_routing_graph)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
    ASSERT_TRUE(clt.update_vehicle_routing_graph());
}

TEST(test_message_lanelet2_translation, get_cur_lanelet_id_by_loc_and_direction)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
    message_services::models::trajectory trajectory;

    //Position within the link lanelet with proper turn direction
    ASSERT_EQ(22414, clt.get_cur_lanelet_id_by_loc_and_direction(38.9549432, -77.1493113, 72, "right", trajectory));
    ASSERT_EQ(22878, clt.get_cur_lanelet_id_by_loc_and_direction(38.9549432, -77.1493113, 72, "straight", trajectory));
    ASSERT_EQ(22528, clt.get_cur_lanelet_id_by_loc_and_direction(38.9549432, -77.1493113, 72, "left", trajectory));

    //Position within the link lanelet without turn direction
    ASSERT_EQ(0, clt.get_cur_lanelet_id_by_loc_and_direction(38.9549432, -77.1493113, 72, "NA", trajectory));
    ASSERT_EQ(0, clt.get_cur_lanelet_id_by_loc_and_direction(38.9549432, -77.1493113, 72, "NA", trajectory));
    ASSERT_EQ(0, clt.get_cur_lanelet_id_by_loc_and_direction(38.9549432, -77.1493113, 72, "NA", trajectory));

    //Position within the entry lanelet
    ASSERT_EQ(19252, clt.get_cur_lanelet_id_by_loc_and_direction(38.9548115, -77.1494016, 72, "NA", trajectory));
    ASSERT_EQ(19252, clt.get_cur_lanelet_id_by_loc_and_direction(38.9548053, -77.1494077, 72, "NA", trajectory));
    ASSERT_EQ(19252, clt.get_cur_lanelet_id_by_loc_and_direction(38.9547239, -77.1494634, 72, "NA", trajectory));

    //Position within the departure lanelet
    ASSERT_EQ(12459, clt.get_cur_lanelet_id_by_loc_and_direction(38.9549304, -77.1491166, 72, "NA", trajectory));
    ASSERT_EQ(12459, clt.get_cur_lanelet_id_by_loc_and_direction(38.9549256, -77.1490945, 72, "NA", trajectory));
}

TEST(test_message_lanelet2_translation, get_cur_lanelet_id_by_point_and_direction)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
    message_services::models::trajectory trajectory;

    //Position within the entry lanelet return 1 lanelet
    lanelet::Point3d point3d = {lanelet::utils::getId(), {-89.162, 316.702, 72}};
    lanelet::BasicPoint3d basic_point3d = point3d.basicPoint();
    ASSERT_EQ(19252, clt.get_cur_lanelet_id_by_point_and_direction(basic_point3d, "", trajectory));

    //Position within the link lanelet with proper turn direction return 1 lanelet
    point3d = {lanelet::utils::getId(), {-87.9078, 320.47, 72}};
    basic_point3d = point3d.basicPoint();
    ASSERT_EQ(22414, clt.get_cur_lanelet_id_by_point_and_direction(basic_point3d, "right", trajectory));
    ASSERT_EQ(0, clt.get_cur_lanelet_id_by_point_and_direction(basic_point3d, "", trajectory));

    //Position within the departure lanelet return 1 lanelet
    point3d = {lanelet::utils::getId(), {-66.3387, 327.636, 72}};
    basic_point3d = point3d.basicPoint();
    ASSERT_EQ(12459, clt.get_cur_lanelet_id_by_point_and_direction(basic_point3d, "", trajectory));
}

TEST(test_message_lanelet2_translation, get_cur_lanelets_by_point)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");

    //Position within the entry lanelet return 1 lanelet
    lanelet::Point3d point3d = {lanelet::utils::getId(), {-89.162, 316.702, 72}};
    lanelet::BasicPoint3d basic_point3d = point3d.basicPoint();
    ASSERT_EQ(1, clt.get_cur_lanelets_by_point(basic_point3d).size());
    ASSERT_EQ(19252, clt.get_cur_lanelets_by_point(basic_point3d).front().id());

    //Position within the link lanelet return 3 lanelets
    point3d = {lanelet::utils::getId(), {-87.9078, 320.47, 72}};
    basic_point3d = point3d.basicPoint();
    ASSERT_EQ(3, clt.get_cur_lanelets_by_point(basic_point3d).size());

    //Position within the departure lanelet return 1 lanelet
    point3d = {lanelet::utils::getId(), {-66.3387, 327.636, 72}};
    basic_point3d = point3d.basicPoint();
    ASSERT_EQ(1, clt.get_cur_lanelets_by_point(basic_point3d).size());
    ASSERT_EQ(12459, clt.get_cur_lanelets_by_point(basic_point3d).front().id());
}

TEST(test_message_lanelet2_translation, distance2_cur_lanelet_end_point)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
    message_services::models::trajectory trajectory;

    //Position within the link lanelet with turn direction
    lanelet::Point3d point3d = {lanelet::utils::getId(), {-83.21549512455204, 329.05751672287005, 72}};
    ASSERT_NEAR(15.033, clt.distance2_cur_lanelet_end(point3d, "right", trajectory), 0.01);

    //Position within the entry lanelet
    point3d = {lanelet::utils::getId(), {-96.40002062426407,304.71216057912693, 72}};
    ASSERT_NEAR(17.4779, clt.distance2_cur_lanelet_end(point3d, "", trajectory), 0.01);

    point3d = {lanelet::utils::getId(), {-91.5717516026476, 313.7486930462219, 72}};
    ASSERT_NEAR(7.12777, clt.distance2_cur_lanelet_end(point3d, "", trajectory), 0.01);

    point3d = {lanelet::utils::getId(), {-91.04298660840287,314.4369775722761, 72}};
    ASSERT_NEAR(6.08981, clt.distance2_cur_lanelet_end(point3d, "", trajectory), 0.01);

    //Position within the departure lanelet
    point3d = {lanelet::utils::getId(), {-66.33865452644577,327.63636981163864, 72}};
    ASSERT_NEAR(137.707, clt.distance2_cur_lanelet_end(point3d, "", trajectory), 0.01);

    point3d = {lanelet::utils::getId(), {-64.42300140950572,327.1034837742677, 72}};
    ASSERT_NEAR(135.699, clt.distance2_cur_lanelet_end(point3d, "", trajectory), 0.01);
}


TEST(test_message_lanelet2_translation, distance2_cur_lanelet_end)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
    message_services::models::trajectory trajectory;

    //Position within the link lanelet with turn direction
    ASSERT_NEAR(15.033, clt.distance2_cur_lanelet_end(38.9549432, -77.1493113, 72, "right", trajectory), 0.01);

    //Position within the entry lanelet
    ASSERT_NEAR(17.4779, clt.distance2_cur_lanelet_end(38.9547239, -77.1494634, 72, "", trajectory), 0.01);
    ASSERT_NEAR(7.12777, clt.distance2_cur_lanelet_end(38.9548053, -77.1494077, 72, "", trajectory), 0.01);
    ASSERT_NEAR(6.08981, clt.distance2_cur_lanelet_end(38.9548115, -77.1494016, 72, "", trajectory), 0.01);

    //Position within the departure lanelet
    ASSERT_NEAR(137.707, clt.distance2_cur_lanelet_end(38.9549304, -77.1491166, 72, "", trajectory), 0.01);
    ASSERT_NEAR(135.699, clt.distance2_cur_lanelet_end(38.9549256, -77.1490945, 72, "", trajectory), 0.01);
}

TEST(test_message_lanelet2_translation, get_lanelet_types_ids_by_route)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
    
    //From lanelet id = 19252  (entry lanelet) to lanelet id = 19252 (entry lanelet)
    std::int64_t start_lanelet_id = 19252;
    std::int64_t dest_lanelet_id = 19252;
    ASSERT_EQ(1, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").size());
    ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").at(start_lanelet_id));

    //From lanelet id = 19252 (entry lanelet) to 22414 (link lanelet) to lanelet id = 12459 (departure lanelet)
    start_lanelet_id = 19252;
    dest_lanelet_id = 12459;
    ASSERT_EQ(3, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").size());
    ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").at(start_lanelet_id));
    ASSERT_EQ(message_services::models::departure, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").at(dest_lanelet_id));

    //From lanelet id = 22414 (link lanelet) to lanelet id = 22414 (link lanelet)
    start_lanelet_id = 22414;
    dest_lanelet_id = 22414;
    ASSERT_EQ(3, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").size());
    ASSERT_EQ(message_services::models::link, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").at(dest_lanelet_id));

    //From lanelet id = 22414 (link lanelet) to lanelet id = 12459 (departure lanelet)
    start_lanelet_id = 22414;
    dest_lanelet_id = 12459;
    ASSERT_EQ(3, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").size());
    ASSERT_EQ(message_services::models::link, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").at(start_lanelet_id));
    ASSERT_EQ(message_services::models::departure, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").at(dest_lanelet_id));

    //From lanelet id = 12459 (departure lanelet) to lanelet id = 12459 (departure lanelet)
    start_lanelet_id = 12459;
    dest_lanelet_id = 12459;
    ASSERT_EQ(1, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").size());
    ASSERT_EQ(message_services::models::unknown, clt.get_lanelet_types_ids_by_route(start_lanelet_id, dest_lanelet_id, "NA").at(dest_lanelet_id));
}

TEST(test_message_lanelet2_translation, get_route_lanelet_ids_by_vehicle_trajectory)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
    message_services::models::trajectory trajectory;
    message_services::models::locationECEF location;
    message_services::models::locationOffsetECEF_t offset;
    std::uint64_t offset_size = 1;
    std::string turn_direction = "";

    //From lanelet 19252 to lanelet 22414 to lanelet 12459
    location.ecef_x = 110460730;
    location.ecef_y = -484214174;
    location.ecef_z = 398846417;
    location.timestamp = 1635804599;
    trajectory.location = location;

    //0.1 sec difference from previous point
    offset.offset_x = 2073;
    offset.offset_y = 1178;
    offset.offset_z = 850;
    trajectory.offsets.push_back(offset);
    ASSERT_EQ(3, clt.get_lanelet_types_ids_by_vehicle_trajectory(trajectory, offset_size, turn_direction).size());

    //From lanelet 22414 to lanelet 12459
    location.ecef_x = 110460800;
    location.ecef_y = -484213915;
    location.ecef_z = 398846710;
    location.timestamp = 1635804599;
    trajectory.location = location;

    //0.1 sec difference from previous point
    offset.offset_x = 2003;
    offset.offset_y = 919;
    offset.offset_z = 557;
    trajectory.offsets.clear();
    trajectory.offsets.push_back(offset);
    turn_direction = "right";
    ASSERT_EQ(2, clt.get_lanelet_types_ids_by_vehicle_trajectory(trajectory, offset_size, turn_direction).size());

    //From lanelet 22528 to lanelet 12459
    turn_direction = "left";
    ASSERT_EQ(0, clt.get_lanelet_types_ids_by_vehicle_trajectory(trajectory, offset_size, turn_direction).size());

    //From lanelet 22878 to lanelet 12459
    turn_direction = "straight";
    ASSERT_EQ(0, clt.get_lanelet_types_ids_by_vehicle_trajectory(trajectory, offset_size, turn_direction).size());

    //From lanelet 12459 to lanelet 12459
    location.ecef_x = 110462803;
    location.ecef_y = -484212996;
    location.ecef_z = 398847267;
    location.timestamp = 1635804599;
    trajectory.location = location;

    //0.1 sec difference from previous point
    offset.offset_x = 0;
    offset.offset_y = 0;
    offset.offset_z = 0;
    trajectory.offsets.clear();
    trajectory.offsets.push_back(offset);
    turn_direction = "";
    ASSERT_EQ(1, clt.get_lanelet_types_ids_by_vehicle_trajectory(trajectory, offset_size, turn_direction).size());
}

TEST(test_message_lanelet2_translation, ecef_2_map_point)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
    message_services::models::trajectory trajectory;
    message_services::models::locationECEF location;
    message_services::models::locationOffsetECEF_t offset;

    //From lanelet 19252 to lanelet 12459
    location.ecef_x = 110460730;
    location.ecef_y = -484214174;
    location.ecef_z = 398846417;
    location.timestamp = 1635804599;
    trajectory.location = location;

    ASSERT_NEAR(-89.162, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).x(), 0.1);
    ASSERT_NEAR(316.702, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).y(), 0.1);
    ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).z(), 0.1);

    //0.1 sec difference from previous point
    offset.offset_x = 2073;
    offset.offset_y = 1178;
    offset.offset_z = 850;
    trajectory.offsets.push_back(offset);

    ASSERT_NEAR(-66.3387, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).x(),  0.1);
    ASSERT_NEAR(327.636, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).y(), 0.1);
    ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).z(), 0.1);

    //From lanelet 22414 to lanelet 12459
    location.ecef_x = 110460800;
    location.ecef_y = -484213915;
    location.ecef_z = 398846710;
    location.timestamp = 1635804599;
    trajectory.location = location;

    ASSERT_NEAR(-87.9078, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).x(), 0.1);
    ASSERT_NEAR(320.47, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).y(), 0.1);
    ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).z(), 0.1);

    //0.1 sec difference from previous point
    offset.offset_x = 2003;
    offset.offset_y = 919;
    offset.offset_z = 557;
    trajectory.offsets.clear();
    trajectory.offsets.push_back(offset);

    ASSERT_NEAR(-66.3387, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).x(), 0.1);
    ASSERT_NEAR(327.636, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).y(), 0.1);
    ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).z(), 0.1);

    //From lanelet 12459 to lanelet 12459
    location.ecef_x = 110462803;
    location.ecef_y = -484212996;
    location.ecef_z = 398847267;
    location.timestamp = 1635804599;
    trajectory.location = location;

    ASSERT_NEAR(-66.3387, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).x(), 0.1);
    ASSERT_NEAR(327.636, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).y(), 0.1);
    ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).z(), 0.1);

    //0.1 sec difference from previous point
    offset.offset_x = 0;
    offset.offset_y = 0;
    offset.offset_z = 0;
    trajectory.offsets.clear();
    trajectory.offsets.push_back(offset);

    ASSERT_NEAR(-66.3387, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).x(), 0.1);
    ASSERT_NEAR(327.636, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).y(), 0.1);
    ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).z(), 0.1);
}
