#include <rapidjson/document.h>

#include "gtest/gtest.h"
#include "message_lanelet2_translation.h"

TEST(test_message_lanelet2_translation, read_lanelet2_map)
{
    message_services::message_translations::message_lanelet2_translation clt;
    ASSERT_TRUE(clt.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm"));
    ASSERT_FALSE(clt.read_lanelet2_map("../fake_map_path.osm"));
}

TEST(test_message_lanelet2_translation, update_vehicle_routing_graph)
{
    message_services::message_translations::message_lanelet2_translation clt("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(clt.update_vehicle_routing_graph());
}

TEST(test_message_lanelet2_translation, get_cur_lanelet_id_by_loc_and_direction)
{
    message_services::message_translations::message_lanelet2_translation clt("../../sample_map/town01_vector_map_test.osm");
    message_services::models::trajectory trajectory;

    //Position within the link lanelet with proper turn direction
    ASSERT_EQ(0, clt.get_cur_lanelet_by_loc_and_direction(48.9977867, 8.0026431, 0, "right", trajectory).id());
    ASSERT_EQ(169, clt.get_cur_lanelet_by_loc_and_direction(48.9977867, 8.0026431, 0, "straight", trajectory).id());
    ASSERT_EQ(155, clt.get_cur_lanelet_by_loc_and_direction(48.9977867, 8.0026431, 0, "left", trajectory).id());

    //Position within the link lanelet without turn direction
    ASSERT_EQ(0, clt.get_cur_lanelet_by_loc_and_direction(48.9977867, 8.0026431, 0, "NA", trajectory).id());


    //Position within the entry lanelet
    ASSERT_EQ(167, clt.get_cur_lanelet_by_loc_and_direction(48.9977278, 8.0026431, 0, "NA", trajectory).id());


    //Position within the departure lanelet
    ASSERT_EQ(168, clt.get_cur_lanelet_by_loc_and_direction(48.9979572, 8.0026431, 0, "NA", trajectory).id());
}



TEST(test_message_lanelet2_translation, distance2_cur_lanelet_end_point)
{
    message_services::message_translations::message_lanelet2_translation clt("../../sample_map/town01_vector_map_test.osm");
    message_services::models::trajectory trajectory;

    //Position within the link lanelet with turn direction
    auto point3d = clt.gps_2_map_point(8.9977867, 8.0026431, 0);
    auto straight_lanelet = clt.get_lanelet_by_id(169);
    auto left_lanelet = clt.get_lanelet_by_id(155);


    ASSERT_NEAR(16.796, clt.distance2_cur_lanelet_end(point3d, left_lanelet,  "left", trajectory), 0.01);
    ASSERT_NEAR(15.854, clt.distance2_cur_lanelet_end(point3d,  straight_lanelet,  "straight", trajectory), 0.01);

    //Positions within the entry lanelet
    point3d = clt.gps_2_map_point(48.9977278, 8.0026431, 0);

    ASSERT_NEAR(5.528, clt.distance2_cur_lanelet_end(point3d, clt.get_lanelet_by_id(167), "", trajectory), 0.01);
    // Futher location in entry lane
    point3d = clt.gps_2_map_point(48.9976419, 8.0026431, 0);
    ASSERT_NEAR(15.07, clt.distance2_cur_lanelet_end(point3d, clt.get_lanelet_by_id(167), "", trajectory), 0.01);
    // Same location in entry lane with direction
    point3d = clt.gps_2_map_point(48.9976419, 8.0026431, 0);
    ASSERT_NEAR(15.07, clt.distance2_cur_lanelet_end(point3d, clt.get_lanelet_by_id(167),  "right", trajectory), 0.01);

    //Position within the departure lanelet
    point3d = clt.gps_2_map_point(48.9976419,8.0025901, 0);
    ASSERT_NEAR(174.876, clt.distance2_cur_lanelet_end(point3d, clt.get_lanelet_by_id(164), "", trajectory), 0.01);

   
}


TEST(test_message_lanelet2_translation, distance2_cur_lanelet_end)
{
    message_services::message_translations::message_lanelet2_translation clt("../../sample_map/town01_vector_map_test.osm");
    message_services::models::trajectory trajectory;

    auto point3d = clt.gps_2_map_point(8.9977867, 8.0026431, 0);
    auto straight_lanelet = clt.get_lanelet_by_id(169);
    auto left_lanelet = clt.get_lanelet_by_id(155);


    ASSERT_NEAR(16.796, clt.distance2_cur_lanelet_end(8.9977867, 8.0026431, 0, left_lanelet,  "left", trajectory), 0.01);
    ASSERT_NEAR(15.854, clt.distance2_cur_lanelet_end(8.9977867, 8.0026431, 0,  straight_lanelet,  "straight", trajectory), 0.01);

    //Positions within the entry lanelet
    point3d = clt.gps_2_map_point(48.9977278, 8.0026431, 0);

    ASSERT_NEAR(5.528, clt.distance2_cur_lanelet_end(48.9977278, 8.0026431, 0, clt.get_lanelet_by_id(167), "", trajectory), 0.01);
    // Futher location in entry lane
    point3d = clt.gps_2_map_point(48.9976419, 8.0026431, 0);
    ASSERT_NEAR(15.07, clt.distance2_cur_lanelet_end(48.9976419, 8.0026431, 0, clt.get_lanelet_by_id(167), "", trajectory), 0.01);
    // Same location in entry lane with direction
    point3d = clt.gps_2_map_point(48.9976419, 8.0026431, 0);
    ASSERT_NEAR(15.07, clt.distance2_cur_lanelet_end(48.9976419, 8.0026431, 0, clt.get_lanelet_by_id(167),  "right", trajectory), 0.01);

    //Position within the departure lanelet
    point3d = clt.gps_2_map_point(48.9976419,8.0025901, 0);
    ASSERT_NEAR(174.876, clt.distance2_cur_lanelet_end(48.9976419,8.0025901, 0, clt.get_lanelet_by_id(164), "", trajectory), 0.01);
}

// TEST(test_message_lanelet2_translation, get_lanelet_types_ids)
// {
//     message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
//     message_services::models::trajectory trajectory;
    
//     lanelet::Point3d point3d = {lanelet::utils::getId(), {-89.162, 316.702, 72}};
//     lanelet::BasicPoint3d basic_point3d = point3d.basicPoint();
//     lanelet::Lanelet lanelet_19252 = clt.get_cur_lanelet_by_point_and_direction(basic_point3d, "", trajectory);
//     ASSERT_EQ(19252, clt.get_cur_lanelet_by_point_and_direction(basic_point3d, "", trajectory).id());

//     //Position within the link lanelet with proper turn direction return 1 lanelet
//     point3d = {lanelet::utils::getId(), {-87.9078, 320.47, 72}};
//     basic_point3d = point3d.basicPoint();
//     lanelet::Lanelet lanelet_22414 = clt.get_cur_lanelet_by_point_and_direction(basic_point3d, "right", trajectory);
//     ASSERT_EQ(22414, clt.get_cur_lanelet_by_point_and_direction(basic_point3d, "right", trajectory).id());
//     ASSERT_EQ(0, clt.get_cur_lanelet_by_point_and_direction(basic_point3d, "", trajectory).id());

//     //Position within the departure lanelet return 1 lanelet
//     point3d = {lanelet::utils::getId(), {-66.3387, 327.636, 72}};
//     basic_point3d = point3d.basicPoint();
//     lanelet::Lanelet lanelet_12459 = clt.get_cur_lanelet_by_point_and_direction(basic_point3d, "", trajectory);
//     ASSERT_EQ(12459, clt.get_cur_lanelet_by_point_and_direction(basic_point3d, "", trajectory).id());

//     //From lanelet id = 19252  (entry lanelet) to lanelet id = 19252 (entry lanelet) without a turn direction.
//     //If turn direction is not provided, it cannot determine which link lanelet inside intersection.
//     ASSERT_EQ(1, clt.get_lanelet_types_ids(lanelet_19252, "NA").size());
//     ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids( lanelet_19252, "NA").at(lanelet_19252.id()));

//     //From lanelet id = 19252 (entry lanelet) to 22414 (link lanelet) to lanelet id = 12459 (departure lanelet)
//     ASSERT_EQ(3, clt.get_lanelet_types_ids(lanelet_19252, "right").size());
//     ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids(lanelet_19252,  "right").at(lanelet_19252.id()));
//     ASSERT_EQ(message_services::models::link, clt.get_lanelet_types_ids(lanelet_19252,  "right").at(lanelet_22414.id()));
//     ASSERT_EQ(message_services::models::departure, clt.get_lanelet_types_ids(lanelet_19252,  "right").at(lanelet_12459.id()));

//     //From lanelet id = 22414 (link lanelet) to lanelet id = 12459 (departure lanelet)
//     ASSERT_EQ(3, clt.get_lanelet_types_ids(lanelet_22414, "NA").size());
//     ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids(lanelet_22414, "NA").at(lanelet_19252.id()));
//     ASSERT_EQ(message_services::models::link, clt.get_lanelet_types_ids(lanelet_22414, "NA").at(lanelet_22414.id()));
//     ASSERT_EQ(message_services::models::departure, clt.get_lanelet_types_ids(lanelet_22414, "NA").at(lanelet_12459.id()));

//     //From lanelet id = 12459 (departure lanelet) to lanelet id = 12459 (departure lanelet)
//     ASSERT_EQ(0, clt.get_lanelet_types_ids(lanelet_12459, "NA").size());
// }

// TEST(test_message_lanelet2_translation, get_route_lanelet_ids)
// {
//     message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
//     message_services::models::trajectory trajectory;
//     message_services::models::locationECEF location;
//     message_services::models::locationOffsetECEF_t offset;
//     std::uint64_t offset_size = 1;
//     std::string turn_direction = "right";

//     //From lanelet 19252 to lanelet 22414 to lanelet 12459
//     location.ecef_x = 110460730;
//     location.ecef_y = -484214174;
//     location.ecef_z = 398846417;
//     location.timestamp = 1635804599;
//     trajectory.location = location;

//     //0.1 sec difference from previous point
//     offset.offset_x = 2073;
//     offset.offset_y = 1178;
//     offset.offset_z = 850;
//     trajectory.offsets.push_back(offset);
//     auto subj_lanelet = clt.get_cur_lanelet_by_point_and_direction(clt.ecef_2_map_point(location.ecef_x,location.ecef_y,location.ecef_z), turn_direction,trajectory);
//     ASSERT_EQ(3, clt.get_lanelet_types_ids(subj_lanelet, turn_direction).size());
//     ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids(subj_lanelet, turn_direction).at(19252));
//     ASSERT_EQ(message_services::models::link, clt.get_lanelet_types_ids(subj_lanelet, turn_direction).at(22414));
//     ASSERT_EQ(message_services::models::departure, clt.get_lanelet_types_ids(subj_lanelet, turn_direction).at(12459));

//     //From link lanelet 22414 to lanelet 12459
//     location.ecef_x = 110460800;
//     location.ecef_y = -484213915;
//     location.ecef_z = 398846710;
//     location.timestamp = 1635804599;
//     trajectory.location = location;

//     //0.1 sec difference from previous point
//     offset.offset_x = 2003;
//     offset.offset_y = 919;
//     offset.offset_z = 557;
//     trajectory.offsets.clear();
//     trajectory.offsets.push_back(offset);
//     turn_direction = "right";
//     subj_lanelet = clt.get_cur_lanelet_by_point_and_direction(clt.ecef_2_map_point(location.ecef_x,location.ecef_y,location.ecef_z),turn_direction,trajectory);
//     ASSERT_EQ(3, clt.get_lanelet_types_ids(subj_lanelet, turn_direction).size());
//     ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[19252]);
//     ASSERT_EQ(message_services::models::link, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[22414]);
//     ASSERT_EQ(message_services::models::departure, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[12459]);

//     //From link lanelet 22528 
//     turn_direction = "left";
//     subj_lanelet = clt.get_cur_lanelet_by_point_and_direction(clt.ecef_2_map_point(location.ecef_x,location.ecef_y,location.ecef_z),turn_direction,trajectory);
//     ASSERT_EQ(3, clt.get_lanelet_types_ids(subj_lanelet, turn_direction).size());
//     ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[19252]);
//     ASSERT_EQ(message_services::models::link, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[22528]);
//     ASSERT_EQ(message_services::models::departure, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[11899]);

//     //From lanelet link 22878 
//     turn_direction = "straight";
//     subj_lanelet = clt.get_cur_lanelet_by_point_and_direction(clt.ecef_2_map_point(location.ecef_x,location.ecef_y,location.ecef_z),turn_direction,trajectory);
//     ASSERT_EQ(3, clt.get_lanelet_types_ids(subj_lanelet, turn_direction).size());
//     ASSERT_EQ(message_services::models::entry, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[19252]);
//     ASSERT_EQ(message_services::models::link, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[22878]);
//     ASSERT_EQ(message_services::models::departure, clt.get_lanelet_types_ids(subj_lanelet, turn_direction)[16377]);

//     //From lanelet 12459 to lanelet 12459
//     location.ecef_x = 110462803;
//     location.ecef_y = -484212996;
//     location.ecef_z = 398847267;
//     location.timestamp = 1635804599;
//     trajectory.location = location;

//     //0.1 sec difference from previous point
//     offset.offset_x = 0;
//     offset.offset_y = 0;
//     offset.offset_z = 0;
//     trajectory.offsets.clear();
//     trajectory.offsets.push_back(offset);
//     turn_direction = "";
//     subj_lanelet = clt.get_cur_lanelet_by_point_and_direction(clt.ecef_2_map_point(location.ecef_x,location.ecef_y,location.ecef_z),turn_direction,trajectory);
//     ASSERT_EQ(12459, subj_lanelet.id());
//     ASSERT_EQ(0, clt.get_lanelet_types_ids(subj_lanelet, turn_direction).size());
// }

// TEST(test_message_lanelet2_translation, ecef_2_map_point)
// {
//     message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");
//     message_services::models::trajectory trajectory;
//     message_services::models::locationECEF location;
//     message_services::models::locationOffsetECEF_t offset;

//     //From lanelet 19252 to lanelet 12459
//     location.ecef_x = 110460730;
//     location.ecef_y = -484214174;
//     location.ecef_z = 398846417;
//     location.timestamp = 1635804599;
//     trajectory.location = location;

//     ASSERT_NEAR(-89.162, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).x(), 0.1);
//     ASSERT_NEAR(316.702, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).y(), 0.1);
//     ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).z(), 0.1);

//     //0.1 sec difference from previous point
//     offset.offset_x = 2073;
//     offset.offset_y = 1178;
//     offset.offset_z = 850;
//     trajectory.offsets.push_back(offset);

//     ASSERT_NEAR(-66.3387, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).x(),  0.1);
//     ASSERT_NEAR(327.636, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).y(), 0.1);
//     ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).z(), 0.1);

//     //From lanelet 22414 to lanelet 12459
//     location.ecef_x = 110460800;
//     location.ecef_y = -484213915;
//     location.ecef_z = 398846710;
//     location.timestamp = 1635804599;
//     trajectory.location = location;

//     ASSERT_NEAR(-87.9078, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).x(), 0.1);
//     ASSERT_NEAR(320.47, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).y(), 0.1);
//     ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).z(), 0.1);

//     //0.1 sec difference from previous point
//     offset.offset_x = 2003;
//     offset.offset_y = 919;
//     offset.offset_z = 557;
//     trajectory.offsets.clear();
//     trajectory.offsets.push_back(offset);

//     ASSERT_NEAR(-66.3387, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).x(), 0.1);
//     ASSERT_NEAR(327.636, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).y(), 0.1);
//     ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).z(), 0.1);

//     //From lanelet 12459 to lanelet 12459
//     location.ecef_x = 110462803;
//     location.ecef_y = -484212996;
//     location.ecef_z = 398847267;
//     location.timestamp = 1635804599;
//     trajectory.location = location;

//     ASSERT_NEAR(-66.3387, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).x(), 0.1);
//     ASSERT_NEAR(327.636, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).y(), 0.1);
//     ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x, location.ecef_y, location.ecef_z).z(), 0.1);

//     //0.1 sec difference from previous point
//     offset.offset_x = 0;
//     offset.offset_y = 0;
//     offset.offset_z = 0;
//     trajectory.offsets.clear();
//     trajectory.offsets.push_back(offset);

//     ASSERT_NEAR(-66.3387, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).x(), 0.1);
//     ASSERT_NEAR(327.636, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).y(), 0.1);
//     ASSERT_NEAR(72, clt.ecef_2_map_point(location.ecef_x + offset.offset_x, location.ecef_y + offset.offset_y, location.ecef_z + offset.offset_z).z(), 0.1);
// }
