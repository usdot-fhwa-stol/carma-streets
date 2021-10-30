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

TEST(test_message_lanelet2_translation, get_cur_lanelet_id_by_pos)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");

    //Position within the link lanelet with proper turn direction
    ASSERT_EQ(22414, clt.get_cur_lanelet_id_by_pos(38.9549432, -77.1493113, 72, "right"));
    ASSERT_EQ(22878, clt.get_cur_lanelet_id_by_pos(38.9549432, -77.1493113, 72, "straight"));
    ASSERT_EQ(22528, clt.get_cur_lanelet_id_by_pos(38.9549432, -77.1493113, 72, "left"));

    //Position within the link lanelet without turn direction
    ASSERT_EQ(0, clt.get_cur_lanelet_id_by_pos(38.9549432, -77.1493113, 72, "NA"));
    ASSERT_EQ(0, clt.get_cur_lanelet_id_by_pos(38.9549432, -77.1493113, 72, "NA"));
    ASSERT_EQ(0, clt.get_cur_lanelet_id_by_pos(38.9549432, -77.1493113, 72, "NA"));

    //Position within the entry lanelet
    ASSERT_EQ(19252, clt.get_cur_lanelet_id_by_pos(38.9548115, -77.1494016, 72, "NA"));
    ASSERT_EQ(19252, clt.get_cur_lanelet_id_by_pos(38.9548053, -77.1494077, 72, "NA"));
    ASSERT_EQ(19252, clt.get_cur_lanelet_id_by_pos(38.9547239, -77.1494634, 72, "NA"));

    //Position within the departure lanelet
    ASSERT_EQ(12459, clt.get_cur_lanelet_id_by_pos(38.9549304, -77.1491166, 72, "NA"));
    ASSERT_EQ(12459, clt.get_cur_lanelet_id_by_pos(38.9549256, -77.1490945, 72, "NA"));
}

TEST(test_message_lanelet2_translation, distance2_cur_lanelet_end)
{
    message_services::message_translations::message_lanelet2_translation clt("../vector_map.osm");

    //Position within the link lanelet without turn direction
    ASSERT_NEAR(15.033, clt.distance2_cur_lanelet_end(38.9549432, -77.1493113, 72, "right"), 0.01);

    //Position within the entry lanelet
    ASSERT_NEAR(17.4779, clt.distance2_cur_lanelet_end(38.9547239, -77.1494634, 72, ""), 0.01);
    ASSERT_NEAR(7.12777, clt.distance2_cur_lanelet_end(38.9548053, -77.1494077, 72, ""), 0.01);
    ASSERT_NEAR(6.08981, clt.distance2_cur_lanelet_end(38.9548115, -77.1494016, 72, ""), 0.01);

    //Position within the departure lanelet
    ASSERT_NEAR(137.707, clt.distance2_cur_lanelet_end(38.9549304, -77.1491166, 72, ""), 0.01);
    ASSERT_NEAR(135.699, clt.distance2_cur_lanelet_end(38.9549256, -77.1490945, 72, ""), 0.01);
}