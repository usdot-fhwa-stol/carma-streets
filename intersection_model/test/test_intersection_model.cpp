#include <gtest/gtest.h>
#include <iomanip>
#include "intersection_model.h"


class intersection_map_mock
{
public:
    intersection_model::intersection_map int_map_msg;
    intersection_map_mock()
    {
        int_map_msg.intersectionid = 9001;
        intersection_model::map_geometry geometry;
        geometry.refpoint.latitude  = 489978505;
        geometry.refpoint.longitude = 80025947;
        geometry.refpoint.elevation = 10;
        geometry.approach.width = 400;

        //Entry lane 1
        intersection_model::map_lane lane;
        lane.lane_id = 1;

        intersection_model::map_node node;
        node.x = -2487;
        node.y = -196;
        lane.nodes.push_back(node);
        node.x= 8;
        node.y = 1;
        lane.nodes.push_back(node);

        intersection_model::map_connection conn;
        conn.lane_id = 4;
        conn.signalGroup = 4;
        lane.connection.push_back(conn);
        conn.lane_id = 5;
        conn.signalGroup = 4;
        lane.connection.push_back(conn);
        geometry.approach.lanes.push_back(lane);

        //Entry lane 2
        lane.lane_id = 2;
        lane.nodes.clear();
        lane.connection.clear();
        node.x= 583; 
        node.y = -1283;
        lane.nodes.push_back(node);
        node.x= -0.17;
        node.y = -251;
        lane.nodes.push_back(node);

        conn.lane_id = 5;
        conn.signalGroup =2;
        lane.connection.push_back(conn);
        conn.lane_id = 6;
        conn.signalGroup =2;
        lane.connection.push_back(conn);
        geometry.approach.lanes.push_back(lane);

        //Entry lane 3
        lane.lane_id = 3;
        lane.nodes.clear();
        lane.connection.clear();
        node.x=  -217;
        node.y = 1708;
        lane.nodes.push_back(node);
        node.x=  0;
        node.y = -451;
        lane.nodes.push_back(node);

        conn.lane_id = 4;
        conn.signalGroup = 3;
        lane.connection.push_back(conn);
        conn.lane_id = 6;
        conn.signalGroup = 3;
        lane.connection.push_back(conn);
        geometry.approach.lanes.push_back(lane);

        //Depart lane 4
        lane.lane_id = 4;
        lane.nodes.clear();
        lane.connection.clear();
        node.x= -217;
        node.y = -1634;
        lane.nodes.push_back(node);
        node.x=  0;
        node.y = -351;
        lane.nodes.push_back(node);
        geometry.approach.lanes.push_back(lane);

        //Depart lane 5
        lane.lane_id = 5;
        lane.nodes.clear();
        lane.connection.clear();
        node.x=  582;
        node.y = 1808;
        lane.nodes.push_back(node);
        node.x=  0;
        node.y = -401;
        lane.nodes.push_back(node);
        geometry.approach.lanes.push_back(lane);

        //Depart lane 6
        lane.lane_id = 6;
        lane.nodes.clear();
        lane.connection.clear();
        node.x=  -1846;
        node.y = 409;
        lane.nodes.push_back(node);
        node.x=  501;
        node.y = 0;
        lane.nodes.push_back(node);
        geometry.approach.lanes.push_back(lane);

        int_map_msg.geometries.push_back(geometry);
    };
    ~intersection_map_mock(){};
};

TEST(intersection_model_test, file_not_found_map) {
    intersection_model::intersection_model model;

    ASSERT_EXIT( model.read_lanelet2_map("../../sample_map/wrong_file.osm"), ::testing::ExitedWithCode(1),"" );
}

TEST(intersection_model_test, read_lanelet2_map)
{
    intersection_model::intersection_model *model = new intersection_model::intersection_model("West Intersection", 9001,"../../sample_map/town01_vector_map_test.osm");
    lanelet::LaneletMapPtr map = model->get_lanelet2_map();
    ASSERT_TRUE(map->laneletLayer.size() > 0);
}

TEST(intersection_model_test, get_lanelet)
{
    intersection_model::intersection_model *model = new intersection_model::intersection_model("West Intersection", 9001,"../../sample_map/town01_vector_map_test.osm");
    lanelet::Lanelet lane = model->get_lanelet(48.9976971,8.0026487);
    ASSERT_EQ(lane.id(), 167);
}


TEST(intersection_model_test, update_vehicle_routing_graph)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.get_vehicleGraph_ptr());
}

TEST(intersection_model_test, update_intersection_info_by_all_way_stop_reg)
{
    intersection_model::intersection_model model;
    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");

    // Define a vector of link lanelet ids;
    std::vector<lanelet::Id> all_link_lanelet_ids_per_intersection; // use all_way_stop reg to update this

    // Find the all_way_stop regulatory element
    lanelet::RegulatoryElementLayer &reg_layer = model.get_lanelet2_map()->regulatoryElementLayer;
    for (auto reg_element_itr = reg_layer.begin(); reg_element_itr != reg_layer.end(); reg_element_itr++)
    {
        lanelet::RegulatoryElement *reg_element_ptr = reg_element_itr->get();

        if (reg_element_ptr->attribute(lanelet::AttributeName::Subtype) == lanelet::AttributeValueString::AllWayStop)
        {
            ASSERT_TRUE(model.update_intersection_info_by_all_way_stop_reg(reg_element_ptr));
            ASSERT_EQ(model.get_intersection_info().link_lanelets_info.size(), 6);
            ASSERT_EQ(model.get_intersection_info().entering_lanelets_info.size(), 3);
            ASSERT_EQ(model.get_intersection_info().departure_lanelets_info.size(), 3);
        }
    }
}

TEST(intersection_model_test, update_link_departure_lanelets_by_entry_lanelet)
{
    intersection_model::intersection_model model;

    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    // Define a vector of link lanelet ids;
    std::vector<lanelet::Id> all_link_lanelet_ids_per_intersection; // use all_way_stop reg to update this

    // Find the all_way_stop regulatory element
    lanelet::LaneletLayer &lanelet_layer = model.get_lanelet2_map()->laneletLayer;
    
    lanelet::Lanelet entry_lanelet = lanelet_layer.get(167);
    ASSERT_TRUE(model.update_link_departure_lanelets_info_by_entry_lanelet(entry_lanelet));
    ASSERT_EQ(model.get_departure_lanelets_info().size(), 2);
    ASSERT_EQ(model.get_link_lanelets_info().size(), 2);

    entry_lanelet = lanelet_layer.get(171);
    ASSERT_TRUE(model.update_link_departure_lanelets_info_by_entry_lanelet(entry_lanelet));

    entry_lanelet = lanelet_layer.get(163);
    ASSERT_TRUE(model.update_link_departure_lanelets_info_by_entry_lanelet(entry_lanelet));
    
    // Once all entry link and departure lanelets are updated by entry lanelet
    
    ASSERT_EQ(model.get_departure_lanelets_info().size(), 3);
    ASSERT_EQ(model.get_link_lanelets_info().size(), 6);
}

TEST(intersection_model_test, get_speed_limit_by_lanelet)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../../sample_map/town01_vector_map_test.osm");
    lanelet::LaneletLayer &lanelet_layer = model.get_lanelet2_map()->laneletLayer;
    lanelet::Lanelet entry_lanelet = lanelet_layer.get(171);
    ASSERT_NEAR(model.get_speed_limit_by_lanelet(entry_lanelet), 11.175, 0.1);
}

TEST(intersection_model_test, get_conflict_lanelets_info)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../../sample_map/town01_vector_map_test.osm");
    model.update_intersection_info();
    
    int link_lanelet_id = 156;
    ASSERT_NE(model.get_conflict_lanelets_info(link_lanelet_id).size(), 0);

    int entry_lanelet_id = 171;
    ASSERT_EQ(model.get_conflict_lanelets_info(entry_lanelet_id).size(), 0);
}

TEST(intersection_model_test, get_shared_entry_link_lanelet_ids)
{   
    intersection_model::intersection_model model("West Intersection", 9001,"../../sample_map/town01_vector_map_test.osm");
    int link_lanelet_id = 155;
    ASSERT_EQ(model.get_shared_entry_link_lanelet_ids(link_lanelet_id).size(), 2);
}

TEST(intersection_model_test, is_valid_link_lanelet_id)
{
    int entry_lanelet_id = 171;
    int link_lanelet_id  = 156;

    intersection_model::intersection_model model;
    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    //Before intersection update, no links yet
    ASSERT_FALSE(model.is_valid_link_lanelet_id(link_lanelet_id));

    model.update_intersection_info();

    ASSERT_FALSE(model.is_valid_link_lanelet_id(entry_lanelet_id));
    ASSERT_TRUE(model.is_valid_link_lanelet_id(link_lanelet_id));
}

TEST(intersection_model_test, update_intersection_info)
{
    intersection_model::intersection_model model;
    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_EQ(model.get_intersection_info().link_lanelets_info.size(), 0);
    ASSERT_EQ(model.get_intersection_info().entering_lanelets_info.size(), 0);
    ASSERT_EQ(model.get_intersection_info().departure_lanelets_info.size(), 0);
    
    ASSERT_TRUE(model.update_intersection_info());

    ASSERT_EQ(model.get_intersection_info().link_lanelets_info.size(), 6);
    ASSERT_EQ(model.get_intersection_info().entering_lanelets_info.size(), 3);
    ASSERT_EQ(model.get_intersection_info().departure_lanelets_info.size(), 3);

    ASSERT_EQ(model.get_entry_lanelets_info().size(), 3);
}

TEST(intersection_model_test, map_point2_gps)
{
    intersection_model::intersection_model model;
    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    double subj_lat = 48.9980241;
    double subj_lon = 8.0025905; //48.9980241, 8.0025905
    auto subj_point = model.gps_2_map_point(subj_lat,subj_lon, 0);
    ASSERT_NEAR(subj_lat, model.map_point2_gps(subj_point.x(), subj_point.y(), subj_point.z()).lat, 0.0000001);
    ASSERT_NEAR(subj_lon, model.map_point2_gps(subj_point.x(), subj_point.y(), subj_point.z()).lon, 0.0000001);    
}

TEST(intersection_model_test, gps_2_map_point)
{   
    intersection_model::intersection_model model;
    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info());
    double subj_lat = 48.9979855;
    double subj_lon = 8.0025914;
    auto subj_point = model.gps_2_map_point(subj_lat,subj_lon, 0);
    ASSERT_NEAR(subj_lat, model.map_point2_gps(subj_point.x(), subj_point.y(), subj_point.z()).lat, 0.0000001);
    ASSERT_NEAR(subj_lon, model.map_point2_gps(subj_point.x(), subj_point.y(), subj_point.z()).lon, 0.0000001);   
}       

TEST(intersection_model_test, update_intersecion_info_by_map_msg)
{
    intersection_map_mock mockData;
    auto int_map_msg_ptr = std::make_shared<intersection_model::intersection_map>();
    int_map_msg_ptr->intersectionid = mockData.int_map_msg.intersectionid;
    int_map_msg_ptr->geometries = mockData.int_map_msg.geometries;
    intersection_model::intersection_model model;
    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info());
    model.update_intersecion_info_by_map_msg(int_map_msg_ptr, 0.0);
    for(auto link_lane: model.get_intersection_info().link_lanelets_info)
    {
        switch (link_lane.id)
        {
            case 169:
            case 155:
                ASSERT_EQ(2,link_lane.signal_group_id);
                break;
            case 165:
            case 156:
                ASSERT_EQ(3,link_lane.signal_group_id);
                break;
            case 161:
            case 160:
                ASSERT_EQ(4,link_lane.signal_group_id);
                break;
        
        default:
            break;
        }
    }
} 

TEST(intersection_model_test, convert_lane_path_2_basic_points)
{
    intersection_model::intersection_model model;
     model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info());   
    intersection_map_mock mockData;
    auto int_map_msg_ptr = std::make_shared<intersection_model::intersection_map>();
    int_map_msg_ptr->intersectionid = mockData.int_map_msg.intersectionid;
    int_map_msg_ptr->geometries = mockData.int_map_msg.geometries;
    auto cur_geometry = int_map_msg_ptr->geometries.front();
    auto ref_point = cur_geometry.refpoint;
    auto lane = cur_geometry.approach.lanes.front();//making sure there are at least two nodes in each lane geometry
    ASSERT_NE(0, model.convert_lane_path_2_basic_points(ref_point, cur_geometry.approach.lanes.front()).size());
}
  
TEST(intersection_model_test, compute_points_2_lanelet_avg_distance)
{
    intersection_model::intersection_model model;
     model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info()); 
    auto enter_ll = model.get_enter_lanelet_by_id(167);
    auto centerline = enter_ll.centerline3d();
    std::vector<lanelet::BasicPoint3d> basic_points3d;
    for (auto itr = centerline.begin(); itr!=centerline.end(); itr++)
    {
        auto p = itr->basicPoint2d();        
        basic_points3d.push_back(lanelet::utils::to3D(p));
    }
    ASSERT_EQ(0, model.compute_points_2_lanelet_avg_distance(basic_points3d, enter_ll));    
}

TEST(intersection_model_test, mapping_lane_id_2_lanelet_id)
{
    intersection_model::intersection_model model;
    intersection_map_mock mockData;
    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info()); 
    auto int_map_msg_ptr = std::make_shared<intersection_model::intersection_map>();
    int_map_msg_ptr->intersectionid = mockData.int_map_msg.intersectionid;
    int_map_msg_ptr->geometries = mockData.int_map_msg.geometries;
    auto cur_geometry = int_map_msg_ptr->geometries.front();
    auto ref_point = cur_geometry.refpoint;
    auto lane2 = cur_geometry.approach.lanes.at(1);//making sure there are at least two nodes in each lane geometry  
    auto enter_ll = model.get_enter_lanelet_by_id(167); 
    std::vector<lanelet::ConstLanelet>  enter_ll_v;
    enter_ll_v.push_back(enter_ll);
    std::unordered_map<long, lanelet::ConstLanelet> lane2lanelet_m;
    model.mapping_lane_id_2_lanelet_id(ref_point,lane2,cur_geometry.approach.width, enter_ll_v,lane2lanelet_m, 0.0);
    ASSERT_EQ(167,lane2lanelet_m[lane2.lane_id].id());
}

TEST(intersection_model_test, get_enter_lanelet_by_id)
{
    intersection_model::intersection_model model;
     model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info());
    ASSERT_EQ(0, model.get_enter_lanelet_by_id(1234).id());
    ASSERT_EQ(167, model.get_enter_lanelet_by_id(167).id());
    ASSERT_EQ(0, model.get_enter_lanelet_by_id(168).id());
}


TEST(intersection_model_test, get_departure_lanelet_by_id)
{
    intersection_model::intersection_model model;
     model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info());
    ASSERT_EQ(0, model.get_link_lanelet_by_id(1234).id());
    ASSERT_EQ(168, model.get_departure_lanelet_by_id(168).id());
}

TEST(intersection_model_test, get_link_lanelet_by_id)
{
    intersection_model::intersection_model model;
     model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info());
    ASSERT_EQ(0, model.get_link_lanelet_by_id(168).id());
    ASSERT_EQ(169, model.get_link_lanelet_by_id(169).id());
}

TEST(intersection_model_test, find_link_lanelet_id_by_enter_depart_lanelet_ids)
{
    intersection_model::intersection_model model;
    model.read_lanelet2_map("../../sample_map/town01_vector_map_test.osm");
    ASSERT_TRUE(model.update_intersection_info());
    ASSERT_EQ(0, model.find_link_lanelet_id_by_enter_depart_lanelet_ids(1234, 1234));
    ASSERT_EQ(169, model.find_link_lanelet_id_by_enter_depart_lanelet_ids(167, 168));
}