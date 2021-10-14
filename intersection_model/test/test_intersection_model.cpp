#include <gtest/gtest.h>
#include "intersection_model.h"

TEST(intersection_model_test, read_lanelet2_map )
{
    intersection_model::intersection_model *model = new intersection_model::intersection_model("../TFHRC_03.01.21.xodr_all_way_stop_active.osm");
    lanelet::LaneletMapPtr map = model->get_lanelet2_map();
    ASSERT_TRUE(map->laneletLayer.size() > 0);

}

TEST(intersection_model_test, get_lanelet )
{
    intersection_model::intersection_model *model = new intersection_model::intersection_model("../TFHRC_03.01.21.xodr_all_way_stop_active.osm");
    lanelet::Lanelet lane = model->get_lanelet( 38.9551104,-77.1498214);
    ASSERT_EQ(lane.id(), 135);
}