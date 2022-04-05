#include <gtest/gtest.h>
#include "intersection_model.h"

TEST(intersection_model_test, read_lanelet2_map)
{
    intersection_model::intersection_model *model = new intersection_model::intersection_model("West Intersection", 9001,"../town01_vector_map.osm");
    lanelet::LaneletMapPtr map = model->get_lanelet2_map();
    ASSERT_TRUE(map->laneletLayer.size() > 0);
}

TEST(intersection_model_test, get_lanelet)
{
    intersection_model::intersection_model *model = new intersection_model::intersection_model("West Intersection", 9001,"../town01_vector_map.osm");
    lanelet::Lanelet lane = model->get_lanelet(38.9551104, -77.1498214);
    ASSERT_EQ(lane.id(), 11899);
}


TEST(intersection_model_test, update_vehicle_routing_graph)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../town01_vector_map.osm");
    ASSERT_FALSE(model.get_vehicleGraph_ptr());
    model.update_vehicle_routing_graph();
    ASSERT_TRUE(model.get_vehicleGraph_ptr());
}

TEST(intersection_model_test, update_intersection_info_by_all_way_stop_reg)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../town01_vector_map.osm");

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
            ASSERT_EQ(model.get_intersection_info().link_lanelets.size(), 12);
            ASSERT_EQ(model.get_intersection_info().entering_lanelets.size(), 4);
            ASSERT_EQ(model.get_intersection_info().departure_lanelets.size(), 4);
        }
    }
}

TEST(intersection_model_test, update_link_departure_lanelets_by_entry_lanelet)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../town01_vector_map.osm");

    // Define a vector of link lanelet ids;
    std::vector<lanelet::Id> all_link_lanelet_ids_per_intersection; // use all_way_stop reg to update this

    // Find the all_way_stop regulatory element
    lanelet::LaneletLayer &lanelet_layer = model.get_lanelet2_map()->laneletLayer;
    
    lanelet::Lanelet entry_lanelet = lanelet_layer.get(19252);
    ASSERT_TRUE(model.update_link_departure_lanelets_by_entry_lanelet(entry_lanelet));
    ASSERT_EQ(model.get_departure_lanelets().size(), 3);
    ASSERT_EQ(model.get_link_lanelets().size(), 3);

    entry_lanelet = lanelet_layer.get(13021);
    ASSERT_TRUE(model.update_link_departure_lanelets_by_entry_lanelet(entry_lanelet));

    entry_lanelet = lanelet_layer.get(16392);
    ASSERT_TRUE(model.update_link_departure_lanelets_by_entry_lanelet(entry_lanelet));
    
    entry_lanelet = lanelet_layer.get(5894);
    ASSERT_TRUE(model.update_link_departure_lanelets_by_entry_lanelet(entry_lanelet));

    ASSERT_EQ(model.get_departure_lanelets().size(), 4);
    ASSERT_EQ(model.get_link_lanelets().size(), 12);
}

TEST(intersection_model_test, get_speed_limit_by_lanelet)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../town01_vector_map.osm");
    lanelet::LaneletLayer &lanelet_layer = model.get_lanelet2_map()->laneletLayer;
    lanelet::Lanelet entry_lanelet = lanelet_layer.get(19252);
    ASSERT_EQ(model.get_speed_limit_by_lanelet(entry_lanelet), 25);
}

TEST(intersection_model_test, get_conflict_lanelets)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../town01_vector_map.osm");
    model.update_intersection_info();
    
    int link_lanelet_id = 22414;
    ASSERT_NE(model.get_conflict_lanelets(link_lanelet_id).size(), 0);

    int entry_lanelet_id = 19252;
    ASSERT_EQ(model.get_conflict_lanelets(entry_lanelet_id).size(), 0);
}

TEST(intersection_model_test, get_shared_entry_link_lanelet_ids)
{   
    intersection_model::intersection_model model("West Intersection", 9001,"../town01_vector_map.osm");
    
    int link_lanelet_id = 22414;
    ASSERT_EQ(model.get_shared_entry_link_lanelet_ids(link_lanelet_id).size(), 3);
}

TEST(intersection_model_test, is_valid_link_lanelet_id)
{
    int entry_lanelet_id = 19252;
    int link_lanelet_id  = 22414;

    intersection_model::intersection_model model("West Intersection", 9001,"../town01_vector_map.osm");
    model.update_intersection_info();

    ASSERT_FALSE(model.is_valid_link_lanelet_id(entry_lanelet_id));
    ASSERT_TRUE(model.is_valid_link_lanelet_id(link_lanelet_id));
}

TEST(intersection_model_test, update_intersection_info)
{
    intersection_model::intersection_model model("West Intersection", 9001,"../town01_vector_map.osm");
    ASSERT_EQ(model.get_intersection_info().link_lanelets.size(), 0);
    ASSERT_EQ(model.get_intersection_info().entering_lanelets.size(), 0);
    ASSERT_EQ(model.get_intersection_info().departure_lanelets.size(), 0);

    ASSERT_TRUE(model.update_intersection_info());

    ASSERT_EQ(model.get_intersection_info().link_lanelets.size(), 12);
    ASSERT_EQ(model.get_intersection_info().entering_lanelets.size(), 4);
    ASSERT_EQ(model.get_intersection_info().departure_lanelets.size(), 4);
}

