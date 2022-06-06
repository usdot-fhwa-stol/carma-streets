#include <gtest/gtest.h>
#include "map_msg_worker.h"

TEST(map_msg_worker_test, constructor)
{
    int intesection_id = 1001;
    std::string bootstrap_server = "localhost:9001";
    std::string group_id = "group_one";
    std::string topic_name = "map_msg_topic";
    intersection_model::map_msg_worker mmw(intesection_id, bootstrap_server, group_id, topic_name);
    ASSERT_EQ(1001, mmw.get_map_msg_ptr()->intersectionid);
}

// TEST(map_msg_worker_test, parse_JSON_string)
// {
//     int intesection_id = 1001;
//     intersection_model::map_msg_worker mmw(intesection_id);
//     std::string payload = "{\"map_data\":{\"msg_issue_revision\":\"3\",\"layer_type\":{\"intersection_data\":[]},\"layer_id\":\"1\",\"intersections\":{\"intersection_geometry\":{\"id\":{\"id\":\"1001\"},\"revision\":\"3\",\"ref_point\":{\"lat\":\"389549844\",\"long\":\"-771493239\",\"elevation\":\"390\"},\"lane_width\":\"274\",\"lane_set\":[{\"lane_id\":\"1\",\"ingress_approach\":\"1\",\"lane_attributes\":{\"directional_use\":\"10\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-523\",\"y\":\"-1294\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"-360\",\"y\":\"-724\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-622\",\"y\":\"-1111\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-209\",\"y\":\"-654\"}}},{\"delta\":{\"node-xy\":{\"x\":\"76\",\"y\":\"-579\"}}},{\"delta\":{\"node-xy\":{\"x\":\"366\",\"y\":\"-509\"}}}]},\"connects_to\":[{\"connecting_lane\":{\"lane\":\"6\"},\"signal_group\":\"2\"},{\"connecting_lane\":{\"lane\":\"7\"},\"signal_group\":\"2\"},{\"connecting_lane\":{\"lane\":\"8\"},\"signal_group\":\"2\"}]},{\"lane_id\":\"5\",\"egress_approach\":\"5\",\"lane_attributes\":{\"directional_use\":\"01\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-965\",\"y\":\"-1056\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"-250\",\"y\":\"-527\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-378\",\"y\":\"-781\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-244\",\"y\":\"-619\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-174\",\"y\":\"-428\"}}},{\"delta\":{\"node-xy\":{\"x\":\"29\",\"y\":\"-729\"}}}]}},{\"lane_id\":\"6\",\"egress_approach\":\"6\",\"lane_attributes\":{\"directional_use\":\"01\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"1523\",\"y\":\"-518\"}}},{\"delta\":{\"node-xy\":{\"x\":\"581\",\"y\":\"-133\"}}},{\"delta\":{\"node-xy\":{\"x\":\"1035\",\"y\":\"-162\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"953\",\"y\":\"-122\"}}},{\"delta\":{\"node-xy\":{\"x\":\"709\",\"y\":\"-41\"}}},{\"delta\":{\"node-xy\":{\"x\":\"1360\",\"y\":\"-98\"}},\"attributes\":{\"delevation\":\"10\"}}]}},{\"lane_id\":\"2\",\"ingress_approach\":\"2\",\"lane_attributes\":{\"directional_use\":\"10\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"1604\",\"y\":\"-182\"}}},{\"delta\":{\"node-xy\":{\"x\":\"593\",\"y\":\"-139\"}}},{\"delta\":{\"node-xy\":{\"x\":\"924\",\"y\":\"-139\"}}},{\"delta\":{\"node-xy\":{\"x\":\"837\",\"y\":\"-110\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"872\",\"y\":\"-93\"}}},{\"delta\":{\"node-xy\":{\"x\":\"1348\",\"y\":\"-64\"}},\"attributes\":{\"delevation\":\"10\"}}]},\"connects_to\":[{\"connecting_lane\":{\"lane\":\"5\"},\"signal_group\":\"4\"},{\"connecting_lane\":{\"lane\":\"7\"},\"signal_group\":\"4\"},{\"connecting_lane\":{\"lane\":\"8\"},\"signal_group\":\"4\"}]},{\"lane_id\":\"7\",\"egress_approach\":\"7\",\"lane_attributes\":{\"directional_use\":\"01\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"872\",\"y\":\"1363\"}}},{\"delta\":{\"node-xy\":{\"x\":\"232\",\"y\":\"498\"}}},{\"delta\":{\"node-xy\":{\"x\":\"308\",\"y\":\"805\"}},\"attributes\":{\"delevation\":\"-10\"}},{\"delta\":{\"node-xy\":{\"x\":\"331\",\"y\":\"741\"}}},{\"delta\":{\"node-xy\":{\"x\":\"250\",\"y\":\"712\"}}}]}},{\"lane_id\":\"3\",\"ingress_approach\":\"3\",\"lane_attributes\":{\"directional_use\":\"10\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"506\",\"y\":\"1467\"}}},{\"delta\":{\"node-xy\":{\"x\":\"349\",\"y\":\"735\"}}},{\"delta\":{\"node-xy\":{\"x\":\"372\",\"y\":\"897\"}},\"attributes\":{\"delevation\":\"-10\"}},{\"delta\":{\"node-xy\":{\"x\":\"285\",\"y\":\"683\"}}},{\"delta\":{\"node-xy\":{\"x\":\"215\",\"y\":\"480\"}}}]},\"connects_to\":[{\"connecting_lane\":{\"lane\":\"5\"},\"signal_group\":\"2\"},{\"connecting_lane\":{\"lane\":\"6\"},\"signal_group\":\"2\"},{\"connecting_lane\":{\"lane\":\"8\"},\"signal_group\":\"2\"}]},{\"lane_id\":\"8\",\"egress_approach\":\"8\",\"lane_attributes\":{\"directional_use\":\"01\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-1540\",\"y\":\"703\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-715\",\"y\":\"214\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-808\",\"y\":\"359\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-587\",\"y\":\"272\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-511\",\"y\":\"255\"}}}]}},{\"lane_id\":\"4\",\"ingress_approach\":\"4\",\"lane_attributes\":{\"directional_use\":\"10\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-1651\",\"y\":\"368\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-465\",\"y\":\"139\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-651\",\"y\":\"208\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-639\",\"y\":\"232\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-750\",\"y\":\"313\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-761\",\"y\":\"492\"}}}]},\"connects_to\":[{\"connecting_lane\":{\"lane\":\"5\"},\"signal_group\":\"4\"},{\"connecting_lane\":{\"lane\":\"6\"},\"signal_group\":\"4\"},{\"connecting_lane\":{\"lane\":\"7\"},\"signal_group\":\"4\"}]},{\"lane_id\":\"9\",\"lane_attributes\":{\"directional_use\":\"00\",\"shared_with\":\"0000000000\",\"lane_type\":{\"crosswalk\":\"0000000000000000\"}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-1023\",\"y\":\"-634\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"808\",\"y\":\"-365\"}}}]}},{\"lane_id\":\"10\",\"lane_attributes\":{\"directional_use\":\"00\",\"shared_with\":\"0000000000\",\"lane_type\":{\"crosswalk\":\"0000000000000000\"}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"1343\",\"y\":\"43\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-134\",\"y\":\"-550\"}}}]}},{\"lane_id\":\"11\",\"lane_attributes\":{\"directional_use\":\"00\",\"shared_with\":\"0000000000\",\"lane_type\":{\"crosswalk\":\"0000000000000000\"}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"238\",\"y\":\"1213\"}}},{\"delta\":{\"node-xy\":{\"x\":\"593\",\"y\":\"-203\"}}}]}},{\"lane_id\":\"12\",\"lane_attributes\":{\"directional_use\":\"00\",\"shared_with\":\"0000000000\",\"lane_type\":{\"crosswalk\":\"0000000000000000\"}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-1174\",\"y\":\"738\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-157\",\"y\":\"-515\"}}}]}}]}}}}";
//     rapidjson::Document doc = mmw.parse_JSON_string(payload);
//     ASSERT_FALSE(doc.HasParseError());

//     std::string invalidJson = "Invalid";
//     doc = mmw.parse_JSON_string(invalidJson);
//     ASSERT_TRUE(doc.HasParseError());
// }

// TEST(map_msg_worker_test, get_map_msg_ptr)
// {
//     int intesection_id = 1001;
//     intersection_model::map_msg_worker mmw(intesection_id);
//     ASSERT_TRUE(mmw.get_map_msg_ptr() != nullptr);
// }

// TEST(map_msg_worker_test, update_map_msg)
// {
//     int intesection_id = 1001;
//     intersection_model::map_msg_worker mmw(intesection_id);
//     std::string payload = "{\"map_data\":{\"msg_issue_revision\":\"3\",\"layer_type\":{\"intersection_data\":[]},\"layer_id\":\"1\",\"intersections\":{\"intersection_geometry\":{\"id\":{\"id\":\"1001\"},\"revision\":\"3\",\"ref_point\":{\"lat\":\"389549844\",\"long\":\"-771493239\",\"elevation\":\"390\"},\"lane_width\":\"274\",\"lane_set\":[{\"lane_id\":\"1\",\"ingress_approach\":\"1\",\"lane_attributes\":{\"directional_use\":\"10\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-523\",\"y\":\"-1294\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"-360\",\"y\":\"-724\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-622\",\"y\":\"-1111\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-209\",\"y\":\"-654\"}}},{\"delta\":{\"node-xy\":{\"x\":\"76\",\"y\":\"-579\"}}},{\"delta\":{\"node-xy\":{\"x\":\"366\",\"y\":\"-509\"}}}]},\"connects_to\":[{\"connecting_lane\":{\"lane\":\"6\"},\"signal_group\":\"2\"},{\"connecting_lane\":{\"lane\":\"7\"},\"signal_group\":\"2\"},{\"connecting_lane\":{\"lane\":\"8\"},\"signal_group\":\"2\"}]},{\"lane_id\":\"5\",\"egress_approach\":\"5\",\"lane_attributes\":{\"directional_use\":\"01\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-965\",\"y\":\"-1056\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"-250\",\"y\":\"-527\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-378\",\"y\":\"-781\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-244\",\"y\":\"-619\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-174\",\"y\":\"-428\"}}},{\"delta\":{\"node-xy\":{\"x\":\"29\",\"y\":\"-729\"}}}]}},{\"lane_id\":\"6\",\"egress_approach\":\"6\",\"lane_attributes\":{\"directional_use\":\"01\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"1523\",\"y\":\"-518\"}}},{\"delta\":{\"node-xy\":{\"x\":\"581\",\"y\":\"-133\"}}},{\"delta\":{\"node-xy\":{\"x\":\"1035\",\"y\":\"-162\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"953\",\"y\":\"-122\"}}},{\"delta\":{\"node-xy\":{\"x\":\"709\",\"y\":\"-41\"}}},{\"delta\":{\"node-xy\":{\"x\":\"1360\",\"y\":\"-98\"}},\"attributes\":{\"delevation\":\"10\"}}]}},{\"lane_id\":\"2\",\"ingress_approach\":\"2\",\"lane_attributes\":{\"directional_use\":\"10\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"1604\",\"y\":\"-182\"}}},{\"delta\":{\"node-xy\":{\"x\":\"593\",\"y\":\"-139\"}}},{\"delta\":{\"node-xy\":{\"x\":\"924\",\"y\":\"-139\"}}},{\"delta\":{\"node-xy\":{\"x\":\"837\",\"y\":\"-110\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"872\",\"y\":\"-93\"}}},{\"delta\":{\"node-xy\":{\"x\":\"1348\",\"y\":\"-64\"}},\"attributes\":{\"delevation\":\"10\"}}]},\"connects_to\":[{\"connecting_lane\":{\"lane\":\"5\"},\"signal_group\":\"4\"},{\"connecting_lane\":{\"lane\":\"7\"},\"signal_group\":\"4\"},{\"connecting_lane\":{\"lane\":\"8\"},\"signal_group\":\"4\"}]},{\"lane_id\":\"7\",\"egress_approach\":\"7\",\"lane_attributes\":{\"directional_use\":\"01\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"872\",\"y\":\"1363\"}}},{\"delta\":{\"node-xy\":{\"x\":\"232\",\"y\":\"498\"}}},{\"delta\":{\"node-xy\":{\"x\":\"308\",\"y\":\"805\"}},\"attributes\":{\"delevation\":\"-10\"}},{\"delta\":{\"node-xy\":{\"x\":\"331\",\"y\":\"741\"}}},{\"delta\":{\"node-xy\":{\"x\":\"250\",\"y\":\"712\"}}}]}},{\"lane_id\":\"3\",\"ingress_approach\":\"3\",\"lane_attributes\":{\"directional_use\":\"10\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"506\",\"y\":\"1467\"}}},{\"delta\":{\"node-xy\":{\"x\":\"349\",\"y\":\"735\"}}},{\"delta\":{\"node-xy\":{\"x\":\"372\",\"y\":\"897\"}},\"attributes\":{\"delevation\":\"-10\"}},{\"delta\":{\"node-xy\":{\"x\":\"285\",\"y\":\"683\"}}},{\"delta\":{\"node-xy\":{\"x\":\"215\",\"y\":\"480\"}}}]},\"connects_to\":[{\"connecting_lane\":{\"lane\":\"5\"},\"signal_group\":\"2\"},{\"connecting_lane\":{\"lane\":\"6\"},\"signal_group\":\"2\"},{\"connecting_lane\":{\"lane\":\"8\"},\"signal_group\":\"2\"}]},{\"lane_id\":\"8\",\"egress_approach\":\"8\",\"lane_attributes\":{\"directional_use\":\"01\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-1540\",\"y\":\"703\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-715\",\"y\":\"214\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-808\",\"y\":\"359\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-587\",\"y\":\"272\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-511\",\"y\":\"255\"}}}]}},{\"lane_id\":\"4\",\"ingress_approach\":\"4\",\"lane_attributes\":{\"directional_use\":\"10\",\"shared_with\":\"0000000000\",\"lane_type\":{\"vehicle\":[]}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-1651\",\"y\":\"368\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-465\",\"y\":\"139\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-651\",\"y\":\"208\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-639\",\"y\":\"232\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-750\",\"y\":\"313\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-761\",\"y\":\"492\"}}}]},\"connects_to\":[{\"connecting_lane\":{\"lane\":\"5\"},\"signal_group\":\"4\"},{\"connecting_lane\":{\"lane\":\"6\"},\"signal_group\":\"4\"},{\"connecting_lane\":{\"lane\":\"7\"},\"signal_group\":\"4\"}]},{\"lane_id\":\"9\",\"lane_attributes\":{\"directional_use\":\"00\",\"shared_with\":\"0000000000\",\"lane_type\":{\"crosswalk\":\"0000000000000000\"}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-1023\",\"y\":\"-634\"}},\"attributes\":{\"delevation\":\"10\"}},{\"delta\":{\"node-xy\":{\"x\":\"808\",\"y\":\"-365\"}}}]}},{\"lane_id\":\"10\",\"lane_attributes\":{\"directional_use\":\"00\",\"shared_with\":\"0000000000\",\"lane_type\":{\"crosswalk\":\"0000000000000000\"}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"1343\",\"y\":\"43\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-134\",\"y\":\"-550\"}}}]}},{\"lane_id\":\"11\",\"lane_attributes\":{\"directional_use\":\"00\",\"shared_with\":\"0000000000\",\"lane_type\":{\"crosswalk\":\"0000000000000000\"}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"238\",\"y\":\"1213\"}}},{\"delta\":{\"node-xy\":{\"x\":\"593\",\"y\":\"-203\"}}}]}},{\"lane_id\":\"12\",\"lane_attributes\":{\"directional_use\":\"00\",\"shared_with\":\"0000000000\",\"lane_type\":{\"crosswalk\":\"0000000000000000\"}},\"node_list\":{\"nodes\":[{\"delta\":{\"node-xy\":{\"x\":\"-1174\",\"y\":\"738\"}}},{\"delta\":{\"node-xy\":{\"x\":\"-157\",\"y\":\"-515\"}}}]}}]}}}}";
//     mmw.update_map_msg(payload);
//     ASSERT_EQ(1001, mmw.get_map_msg_ptr()->intersectionid);
//     ASSERT_EQ(389549844, mmw.get_map_msg_ptr()->geometries.front().refpoint.latitude);
//     ASSERT_EQ(-771493239, mmw.get_map_msg_ptr()->geometries.front().refpoint.longitude);
//     ASSERT_EQ(390, mmw.get_map_msg_ptr()->geometries.front().refpoint.elevation);
//     ASSERT_EQ(274, mmw.get_map_msg_ptr()->geometries.front().approach.width);
//     ASSERT_EQ(12, mmw.get_map_msg_ptr()->geometries.front().approach.lanes.size());
//     for (auto lane : mmw.get_map_msg_ptr()->geometries.front().approach.lanes)
//     {
//         switch (lane.lane_id)
//         {
//         case 1:
//             ASSERT_EQ(3, lane.connection.size());
//             ASSERT_EQ(6, lane.connection[0].lane_id);
//             ASSERT_EQ(7, lane.connection[1].lane_id);
//             ASSERT_EQ(8, lane.connection[2].lane_id);
//             ASSERT_EQ(2, lane.connection[0].signalGroup);
//             ASSERT_EQ(2, lane.connection[1].signalGroup);
//             ASSERT_EQ(2, lane.connection[2].signalGroup);
//             ASSERT_EQ(-523, lane.nodes[0].x);
//             ASSERT_EQ(-1294, lane.nodes[0].y);
//             ASSERT_EQ(366, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(-509, lane.nodes[lane.nodes.size() - 1].y);
//             break;
//         case 2:
//             ASSERT_EQ(3, lane.connection.size());
//             ASSERT_EQ(5, lane.connection[0].lane_id);
//             ASSERT_EQ(7, lane.connection[1].lane_id);
//             ASSERT_EQ(8, lane.connection[2].lane_id);
//             ASSERT_EQ(4, lane.connection[0].signalGroup);
//             ASSERT_EQ(4, lane.connection[1].signalGroup);
//             ASSERT_EQ(4, lane.connection[2].signalGroup);
//             break;
//         case 3:
//             ASSERT_EQ(3, lane.connection.size());
//             ASSERT_EQ(5, lane.connection[0].lane_id);
//             ASSERT_EQ(6, lane.connection[1].lane_id);
//             ASSERT_EQ(8, lane.connection[2].lane_id);
//             ASSERT_EQ(2, lane.connection[0].signalGroup);
//             ASSERT_EQ(2, lane.connection[1].signalGroup);
//             ASSERT_EQ(2, lane.connection[2].signalGroup);
//             break;
//         case 4:
//             ASSERT_EQ(3, lane.connection.size());
//             ASSERT_EQ(5, lane.connection[0].lane_id);
//             ASSERT_EQ(6, lane.connection[1].lane_id);
//             ASSERT_EQ(7, lane.connection[2].lane_id);
//             ASSERT_EQ(4, lane.connection[0].signalGroup);
//             ASSERT_EQ(4, lane.connection[1].signalGroup);
//             ASSERT_EQ(4, lane.connection[2].signalGroup);
//             break;
//         case 5:
//             ASSERT_EQ(-965, lane.nodes[0].x);
//             ASSERT_EQ(-1056, lane.nodes[0].y);
//             ASSERT_EQ(29, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(-729, lane.nodes[lane.nodes.size() - 1].y);
//             break;
//         case 6:

//             ASSERT_EQ(1523, lane.nodes[0].x);
//             ASSERT_EQ(-518, lane.nodes[0].y);
//             ASSERT_EQ(1360, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(-98, lane.nodes[lane.nodes.size() - 1].y);
//             break;
//         case 7:
//             ASSERT_EQ(872, lane.nodes[0].x);
//             ASSERT_EQ(1363, lane.nodes[0].y);
//             ASSERT_EQ(250, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(712, lane.nodes[lane.nodes.size() - 1].y);
//             break;
//         case 8:
//             ASSERT_EQ(-1540, lane.nodes[0].x);
//             ASSERT_EQ(703, lane.nodes[0].y);
//             ASSERT_EQ(-511, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(255, lane.nodes[lane.nodes.size() - 1].y);
//             break;
//         case 9:
//             ASSERT_EQ(-1023, lane.nodes[0].x);
//             ASSERT_EQ(-634, lane.nodes[0].y);
//             ASSERT_EQ(808, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(-365, lane.nodes[lane.nodes.size() - 1].y);
//             break;
//         case 10:
//             ASSERT_EQ(1343, lane.nodes[0].x);
//             ASSERT_EQ(43, lane.nodes[0].y);
//             ASSERT_EQ(-134, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(-550, lane.nodes[lane.nodes.size() - 1].y);
//             break;
//         case 11:
//             ASSERT_EQ(238, lane.nodes[0].x);
//             ASSERT_EQ(1213, lane.nodes[0].y);
//             ASSERT_EQ(593, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(-203, lane.nodes[lane.nodes.size() - 1].y);
//             break;
//         case 12:
//             ASSERT_EQ(-1174, lane.nodes[0].x);
//             ASSERT_EQ(738, lane.nodes[0].y);
//             ASSERT_EQ(-157, lane.nodes[lane.nodes.size() - 1].x);
//             ASSERT_EQ(-515, lane.nodes[lane.nodes.size() - 1].y);
//             break;

//         default:
//             break;
//         }
//     }
// }
