
#include "gtest/gtest.h"
#include <vector>
#include "rapidjson/document.h"
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

#include "intersection_client.h"

/* an example of the west intersection at TFHRC
* 
{
    "departure_lanelets": [
        {
            "id": 11899,
            "length": 168.0130065619106,
            "speed_limit": 25
        },
        {
            "id": 12459,
            "length": 139.71480827232895,
            "speed_limit": 25
        },
        {
            "id": 16349,
            "length": 252.00748318937997,
            "speed_limit": 25
        },
        {
            "id": 16377,
            "length": 5.920239222756028,
            "speed_limit": 25
        }
    ],
    "entry_lanelets": [
        {
            "id": 5894,
            "length": 166.61071357210818,
            "speed_limit": 25
        },
        {
            "id": 19252,
            "length": 239.8257606719354,
            "speed_limit": 25
        },
        {
            "id": 13021,
            "length": 138.492139313013,
            "speed_limit": 25
        },
        {
            "id": 16392,
            "length": 5.6726429517525645,
            "speed_limit": 25
        }
    ],
    "id": 9001,
    "link_lanelets": [
        {
            "conflict_lanelet_ids": [
                22878,
                22528,
                22414,
                22284,
                23266,
                22760
            ],
            "id": 23016,
            "length": 30.232082525062513,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                22284,
                22760
            ],
            "id": 22642,
            "length": 20.381006432868563,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                22878,
                22528,
                23378,
                23154,
                23266,
                22760
            ],
            "id": 23578,
            "length": 23.42591790152499,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                23016,
                23578,
                23378,
                23154,
                22284,
                23266
            ],
            "id": 22878,
            "length": 24.384711000255873,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                23016,
                23578,
                23154,
                22284,
                23490,
                22760
            ],
            "id": 22528,
            "length": 26.105058528196754,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                23016,
                23266
            ],
            "id": 22414,
            "length": 24.571152180752847,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                23578,
                22878
            ],
            "id": 23378,
            "length": 20.810809309758536,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                23578,
                22878,
                22528,
                23490,
                23266,
                22760
            ],
            "id": 23154,
            "length": 29.695683592800144,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                23016,
                22642,
                22878,
                22528,
                23266,
                22760
            ],
            "id": 22284,
            "length": 29.78435946604254,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                22528,
                23154
            ],
            "id": 23490,
            "length": 20.274508732005305,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                23016,
                23578,
                22878,
                22414,
                23154,
                22284
            ],
            "id": 23266,
            "length": 25.85629063026488,
            "speed_limit": 25
        },
        {
            "conflict_lanelet_ids": [
                23016,
                22642,
                23578,
                22528,
                23154,
                22284
            ],
            "id": 22760,
            "length": 24.294078718906057,
            "speed_limit": 25
        }
    ],
    "name": "West Intersection"
}
*/


TEST(test_intersection_client, call)
{

    /* this unit test is conducted for the west intersection at TFHRC */
    intersection_client localmap;
    localmap.call();

    ASSERT_EQ(9001, localmap.get_intersectionId());
    ASSERT_EQ("West Intersection", localmap.get_intersectionName());
    ASSERT_EQ(20, localmap.get_laneCount());
    ASSERT_EQ(20, localmap.get_laneIdAll().size());
    ASSERT_EQ(4, localmap.get_laneIdEntry().size());
    ASSERT_EQ(4, localmap.get_laneIdExit().size());
    ASSERT_EQ(12, localmap.get_laneIdLink().size());

    vector<string> lane_types{"entry", "exit", "link"};
    vector<string> lane_directions{"straight", "right", "left", "unknown"};
    for (int i = 0; i < (int)localmap.get_laneIdAll().size(); ++i){
        string lane_id = localmap.get_laneIdAll()[i];
        ASSERT_LT(0, stoi(lane_id));
        ASSERT_EQ(i, localmap.get_laneIndex(lane_id));
        ASSERT_TRUE(find(lane_types.begin(), lane_types.end(), localmap.get_laneType(lane_id)) != lane_types.end());
        ASSERT_TRUE(find(lane_directions.begin(), lane_directions.end(), localmap.get_laneDirection(lane_id)) != lane_directions.end());
        ASSERT_LT(0.0, localmap.get_laneLength(lane_id));
        ASSERT_LT(0.0, localmap.get_laneSpeedLimit(lane_id));
        if (localmap.get_laneType(lane_id) == "entry" || localmap.get_laneType(lane_id) == "exit"){
            ASSERT_EQ(-1, localmap.get_lanePriority(lane_id));
        } 
        else if (localmap.get_laneType(lane_id) == "link"){
            ASSERT_LT(0, localmap.get_lanePriority(lane_id));
        }
    }

    // test the conflict status. the used example links are: 23016, 22878, and 22528
    ASSERT_EQ(true, localmap.hasConflict("23016", "23016"));
    ASSERT_EQ(true, localmap.hasConflict("23016", "22878"));
    ASSERT_EQ(true, localmap.hasConflict("23016", "22528"));
    ASSERT_EQ(false, localmap.hasConflict("22878", "22528"));

}