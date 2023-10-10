#include "gtest/gtest.h"
#include <deserializers/sensor_data_sharing_msg_json_deserializer.hpp>
#include <string>
using namespace streets_utils::messages;

TEST(sensor_dara_sharing_msg_json_deserialization_test, deserialize) {
    
    std::string json = "{"
        "\"equipment_type\": 0,"
        "\"msg_cnt\": 255,"
        "\"ref_pos\": {"
        "    \"long\": 600000000,"
        "    \"elevation\": 30,"
        "    \"lat\": 400000000"
        "},"
        "\"ref_pos_el_conf\": 10,"
        "\"ref_pos_xy_conf\": {"
        "    \"orientation\": 25000,"
        "    \"semi_major\": 235,"
        "    \"semi_minor\": 200"
        "},"
        "\"sdsm_time_stamp\": {"
        "    \"day\": 4,"
        "    \"hour\": 19,"
        "    \"minute\": 15,"
        "    \"month\": 7,"
        "    \"offset\": 400,"
        "    \"second\": 5000,"
        "    \"year\": 2007"
        "},"
        "\"source_id\": \"0102C304\","
        "\"objects\": ["
        "    {"
        "    \"detected_object_data\": {"
        "        \"detected_object_common_data\": {"
        "        \"acc_cfd_x\": 4,"
        "        \"acc_cfd_y\": 5,"
        "        \"acc_cfd_yaw\": 3,"
        "        \"acc_cfd_z\": 6,"
        "        \"accel_4_way\": {"
        "            \"lat\": -500,"
        "            \"long\": 200,"
        "            \"vert\": 1,"
        "            \"yaw\": 400"
        "        },"
        "        \"heading\": 16000,"
        "        \"heading_conf\": 4,"
        "        \"measurement_time\": -1100,"
        "        \"object_id\": 12200,"
        "        \"obj_type\": 1,"
        "        \"obj_type_cfd\": 65,"
        "        \"pos\": {"
        "            \"offset_x\": 4000,"
        "            \"offset_y\": -720,"
        "            \"offset_z\": 20"
        "        },"
        "        \"pos_confidence\": {"
        "            \"elevation\": 5,"
        "            \"pos\": 2"
        "        },"
        "        \"speed\": 2100,"
        "        \"speed_confidence\": 3,"
        "        \"speed_confidence_z\": 4,"
        "        \"speed_z\": 1000,"
        "        \"time_confidence\": 2"
        "        }"
        "    }"
        "    }"
        "    ]"
        "}";
    auto msg = from_json(json );

    EXPECT_EQ(255, msg._msg_count);
    EXPECT_EQ("0102C304", msg._source_id);
    EXPECT_EQ(equipment_type::UNKNOWN, msg._equipment_type);
    EXPECT_EQ(400, msg._time_stamp.offset);
    EXPECT_EQ(5000, msg._time_stamp.second);
    EXPECT_EQ(15, msg._time_stamp.minute);
    EXPECT_EQ(19, msg._time_stamp.hour);
    EXPECT_EQ(4, msg._time_stamp.day);
    EXPECT_EQ(7, msg._time_stamp.month);
    EXPECT_EQ(2007, msg._time_stamp.year);
}