// Copyright 2019-2023 Leidos
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <gtest/gtest.h>
#include <deserializers/sensor_data_sharing_msg_json_deserializer.hpp>
#include <string>
using namespace streets_utils::messages;

TEST(sensor_dara_sharing_msg_json_deserialization_test, deserialize) {
    
    std::string json = "{"
        "\"equipment_type\": 0,"
        "\"msg_cnt\": 255,"
        "\"ref_pos\": {"
        "    \"long\": 1800000001,"
        "    \"elevation\": 30,"
        "    \"lat\": 900000001"
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
    // Confirm timestamp
    EXPECT_EQ(400, msg._time_stamp.offset);
    EXPECT_EQ(5000, msg._time_stamp.second);
    EXPECT_EQ(15, msg._time_stamp.minute);
    EXPECT_EQ(19, msg._time_stamp.hour);
    EXPECT_EQ(4, msg._time_stamp.day);
    EXPECT_EQ(7, msg._time_stamp.month);
    EXPECT_EQ(2007, msg._time_stamp.year);
    // Confirm reference position
    EXPECT_EQ(1800000001,msg._ref_positon._longitude);
    EXPECT_EQ(900000001, msg._ref_positon._latitude );
    // Confirm optional elevation is present
    EXPECT_TRUE(msg._ref_positon._elevation.has_value());
    EXPECT_EQ(30, msg._ref_positon._elevation);
    // Confirm positional accuracy
    EXPECT_EQ(235, msg._ref_position_confidence._semi_major_axis_accuracy);
    EXPECT_EQ(200, msg._ref_position_confidence._semi_minor_axis_accuracy);
    EXPECT_EQ(25000, msg._ref_position_confidence._semi_major_axis_orientation);
    // Get Detection
    ASSERT_EQ(1, msg._objects.size());
    auto detection = msg._objects[0];
    // Confirm Common Data
    EXPECT_EQ(12200 ,detection._detected_object_common_data._object_id);
    EXPECT_EQ(object_type_from_int(1) ,detection._detected_object_common_data._object_type);
    EXPECT_EQ(65,detection._detected_object_common_data._classification_confidence);
    // Confirm 4 way accel
    EXPECT_EQ( -500, detection._detected_object_common_data._acceleration_4_way->_lateral_accel);
    EXPECT_EQ( 200, detection._detected_object_common_data._acceleration_4_way->_longitudinal_accel);
    EXPECT_EQ( 1, detection._detected_object_common_data._acceleration_4_way->_vertical_accel);
    EXPECT_EQ( 400, detection._detected_object_common_data._acceleration_4_way->_yaw_rate);
    // Confirm acceleration confidence
    EXPECT_EQ( acceleration_confidence_from_int(4), detection._detected_object_common_data._lateral_acceleration_confidence);
    EXPECT_EQ( acceleration_confidence_from_int(5), detection._detected_object_common_data._longitudinal_acceleration_confidence);
    EXPECT_EQ( acceleration_confidence_from_int(6), detection._detected_object_common_data._vertical_accelaration_confidence);
    EXPECT_EQ( angular_velocity_confidence_from_int(3), detection._detected_object_common_data._yaw_rate_confidence);
    // Confirm position



}

TEST(sensor_dara_sharing_msg_json_deserialization_test, deserialize_optional_obstacle_data) {
    
    std::string json = "{"
    "    \"equipment_type\": 1,"
    "    \"msg_cnt\": 1,"
    "    \"objects\": ["
    "        {"
    "        \"detected_object_data\": {"
    "            \"detected_object_common_data\": {"
    "            \"acc_cfd_x\": 4,"
    "            \"acc_cfd_y\": 5,"
    "            \"acc_cfd_yaw\": 3,"
    "            \"acc_cfd_z\": 6,"
    "            \"accel_4_way\": {"
    "                \"lat\": -500,"
    "                \"long\": 200,"
    "                \"vert\": 1,"
    "                \"yaw\": 400"
    "            },"
    "            \"heading\": 16000,"
    "            \"heading_conf\": 4,"
    "            \"measurement_time\": -1100,"
    "            \"object_id\": 12200,"
    "            \"obj_type\": 1,"
    "            \"obj_type_cfd\": 65,"
    "            \"pos\": {"
    "                \"offset_x\": 4000,"
    "                \"offset_y\": -720,"
    "                \"offset_z\": 20"
    "            },"
    "            \"pos_confidence\": {"
    "                \"elevation\": 5,"
    "                \"pos\": 2"
    "            },"
    "            \"speed\": 2100,"
    "            \"speed_confidence\": 3,"
    "            \"speed_confidence_z\": 4,"
    "            \"speed_z\": 1000,"
    "            \"time_confidence\": 2"
    "            },"
    "            \"detected_object_optional_data\": {"
    "            \"detected_obstacle_data\": {"
    "                \"obst_size\": {"
    "                \"height\": 100,"
    "                \"length\": 300,"
    "                \"width\": 400"
    "                },"
    "                \"obst_size_confidence\": {"
    "                \"height_confidence\": 8,"
    "                \"length_confidence\": 7,"
    "                \"width_confidence\": 6"
    "                }"
    "            }"
    "            }"
    "        }"
    "        }"
    "    ],"
    "    \"ref_pos\": {"
    "        \"long\": 600000000,"
    "        \"elevation\": 30,"
    "        \"lat\": 400000000"
    "    },"
    "    \"ref_pos_el_conf\": 10,"
    "    \"ref_pos_xy_conf\": {"
    "        \"orientation\": 25000,"
    "        \"semi_major\": 235,"
    "        \"semi_minor\": 200"
    "    },"
    "    \"sdsm_time_stamp\": {"
    "        \"day\": 4,"
    "        \"hour\": 19,"
    "        \"minute\": 15,"
    "        \"month\": 7,"
    "        \"offset\": 400,"
    "        \"second\": 5000,"
    "        \"year\": 2007"
    "    },"
    "    \"source_id\": \"0102C304\""
    "    }";
    auto msg = from_json(json );

    EXPECT_EQ(1, msg._msg_count);
    EXPECT_EQ("0102C304", msg._source_id);
    EXPECT_EQ(equipment_type::RSU, msg._equipment_type);
    // Confirm timestamp
    EXPECT_EQ(400, msg._time_stamp.offset);
    EXPECT_EQ(5000, msg._time_stamp.second);
    EXPECT_EQ(15, msg._time_stamp.minute);
    EXPECT_EQ(19, msg._time_stamp.hour);
    EXPECT_EQ(4, msg._time_stamp.day);
    EXPECT_EQ(7, msg._time_stamp.month);
    EXPECT_EQ(2007, msg._time_stamp.year);
    // Confirm reference position
    EXPECT_EQ(600000000,msg._ref_positon._longitude);
    EXPECT_EQ(400000000, msg._ref_positon._latitude );
    // Confirm optional elevation is present
    EXPECT_TRUE(msg._ref_positon._elevation.has_value());
    EXPECT_EQ(30, msg._ref_positon._elevation);
    // Confirm positional accuracy
    EXPECT_EQ(235, msg._ref_position_confidence._semi_major_axis_accuracy);
    EXPECT_EQ(200, msg._ref_position_confidence._semi_minor_axis_accuracy);
    EXPECT_EQ(25000, msg._ref_position_confidence._semi_major_axis_orientation);


}

TEST(sensor_dara_sharing_msg_json_deserialization_test, deserialize_optional_vru_data) {
    
    std::string json = "{"
    "    \"equipment_type\": 1,"
    "    \"msg_cnt\": 1,"
    "    \"objects\": ["
    "    {"
    "        \"detected_object_data\": {"
    "        \"detected_object_common_data\": {"
    "            \"acc_cfd_x\": 4,"
    "            \"acc_cfd_y\": 5,"
    "            \"acc_cfd_yaw\": 3,"
    "            \"acc_cfd_z\": 6,"
    "            \"accel_4_way\": {"
    "            \"lat\": -500,"
    "            \"long\": 200,"
    "            \"vert\": 1,"
    "            \"yaw\": 400"
    "            },"
    "            \"heading\": 16000,"
    "            \"heading_conf\": 4,"
    "            \"measurement_time\": -1100,"
    "            \"object_id\": 12200,"
    "            \"obj_type\": 1,"
    "            \"obj_type_cfd\": 65,"
    "            \"pos\": {"
    "            \"offset_x\": 4000,"
    "            \"offset_y\": -720,"
    "            \"offset_z\": 20"
    "            },"
    "            \"pos_confidence\": {"
    "            \"elevation\": 5,"
    "            \"pos\": 2"
    "            },"
    "            \"speed\": 2100,"
    "            \"speed_confidence\": 3,"
    "            \"speed_confidence_z\": 4,"
    "            \"speed_z\": 1000,"
    "            \"time_confidence\": 2"
    "        },"
    "        \"detected_object_optional_data\": {"
    "            \"detected_vru_data\": {"
    "            \"attachment\": 3,"
    "            \"basic_type\": 1,"
    "            \"propulsion\": {"
    "                \"human\": 2"
    "            },"
    "            \"radius\": 30"
    "            }"
    "        }"
    "        }"
    "    }"
    "    ],"
    "    \"ref_pos\": {"
    "    \"long\": 600000000,"
    "    \"elevation\": 30,"
    "    \"lat\": 400000000"
    "    },"
    "    \"ref_pos_el_conf\": 10,"
    "    \"ref_pos_xy_conf\": {"
    "    \"orientation\": 25000,"
    "    \"semi_major\": 235,"
    "    \"semi_minor\": 200"
    "    },"
    "    \"sdsm_time_stamp\": {"
    "    \"day\": 4,"
    "    \"hour\": 19,"
    "    \"minute\": 15,"
    "    \"month\": 7,"
    "    \"offset\": 400,"
    "    \"second\": 5000,"
    "    \"year\": 2007"
    "    },"
    "    \"source_id\": \"0102C304\""
    "    }";
    auto msg = from_json(json );

    EXPECT_EQ(1, msg._msg_count);
    EXPECT_EQ("0102C304", msg._source_id);
    EXPECT_EQ(equipment_type::RSU, msg._equipment_type);
    // Confirm timestamp
    EXPECT_EQ(400, msg._time_stamp.offset);
    EXPECT_EQ(5000, msg._time_stamp.second);
    EXPECT_EQ(15, msg._time_stamp.minute);
    EXPECT_EQ(19, msg._time_stamp.hour);
    EXPECT_EQ(4, msg._time_stamp.day);
    EXPECT_EQ(7, msg._time_stamp.month);
    EXPECT_EQ(2007, msg._time_stamp.year);
    // Confirm reference position
    EXPECT_EQ(600000000,msg._ref_positon._longitude);
    EXPECT_EQ(400000000, msg._ref_positon._latitude );
    // Confirm optional elevation is present
    EXPECT_TRUE(msg._ref_positon._elevation.has_value());
    EXPECT_EQ(30, msg._ref_positon._elevation);
    // Confirm positional accuracy
    EXPECT_EQ(235, msg._ref_position_confidence._semi_major_axis_accuracy);
    EXPECT_EQ(200, msg._ref_position_confidence._semi_minor_axis_accuracy);
    EXPECT_EQ(25000, msg._ref_position_confidence._semi_major_axis_orientation);


}

TEST(sensor_dara_sharing_msg_json_deserialization_test, deserialize_optional_vehicle_data) {
    
    std::string json = "{"
    "    \"equipment_type\": 1,"
    "    \"msg_cnt\": 1,"
    "    \"objects\": ["
    "        {"
    "        \"detected_object_data\": {"
    "            \"detected_object_common_data\": {"
    "            \"acc_cfd_x\": 4,"
    "            \"acc_cfd_y\": 5,"
    "            \"acc_cfd_yaw\": 3,"
    "            \"acc_cfd_z\": 6,"
    "            \"accel_4_way\": {"
    "                \"lat\": -500,"
    "                \"long\": 200,"
    "                \"vert\": 1,"
    "                \"yaw\": 400"
    "            },"
    "            \"heading\": 16000,"
    "            \"heading_conf\": 4,"
    "            \"measurement_time\": -1100,"
    "            \"object_id\": 12200,"
    "            \"obj_type\": 1,"
    "            \"obj_type_cfd\": 65,"
    "            \"pos\": {"
    "                \"offset_x\": 4000,"
    "                \"offset_y\": -720,"
    "                \"offset_z\": 20"
    "            },"
    "            \"pos_confidence\": {"
    "                \"elevation\": 5,"
    "                \"pos\": 2"
    "            },"
    "            \"speed\": 2100,"
    "            \"speed_confidence\": 3,"
    "            \"speed_confidence_z\": 4,"
    "            \"speed_z\": 1000,"
    "            \"time_confidence\": 2"
    "            },"
    "            \"detected_object_optional_data\": {"
    "            \"detected_vehicle_data\": {"
    "                \"height\": 70,"
    "                \"lights\": 8,"
    "                \"size\": {"
    "                \"length\": 700,"
    "                \"width\": 300"
    "                },"
    "                \"veh_ang_vel\": {"
    "                \"pitch_rate\": 600,"
    "                \"roll_rate\": -800"
    "                },"
    "                \"veh_ang_vel_confidence\": {"
    "                \"pitch_rate_confidence\": 3,"
    "                \"roll_rate_confidence\": 4"
    "                },"
    "                \"veh_attitude\": {"
    "                \"pitch\": 2400,"
    "                \"roll\": -12000,"
    "                \"yaw\": 400"
    "                },"
    "                \"veh_attitude_confidence\": {"
    "                \"pitch_confidence\": 2,"
    "                \"roll_confidence\": 3,"
    "                \"yaw_confidence\": 4"
    "                },"
    "                \"vehicle_class\": 11,"
    "                \"vehicle_class_conf\": 75,"
    "                \"vehicle_size_confidence\": {"
    "                \"vehicle_height_confidence\": 5,"
    "                \"vehicle_length_confidence\": 6,"
    "                \"vehicle_width_confidence\": 7"
    "                }"
    "            }"
    "            }"
    "        }"
    "        }"
    "    ],"
    "    \"ref_pos\": {"
    "        \"long\": 600000000,"
    "        \"elevation\": 30,"
    "        \"lat\": 400000000"
    "    },"
    "    \"ref_pos_el_conf\": 10,"
    "    \"ref_pos_xy_conf\": {"
    "        \"orientation\": 25000,"
    "        \"semi_major\": 235,"
    "        \"semi_minor\": 200"
    "    },"
    "    \"sdsm_time_stamp\": {"
    "        \"day\": 4,"
    "        \"hour\": 19,"
    "        \"minute\": 15,"
    "        \"month\": 7,"
    "        \"offset\": 400,"
    "        \"second\": 5000,"
    "        \"year\": 2007"
    "    },"
    "    \"source_id\": \"0102C304\""
    "    }";
    auto msg = from_json(json );

    EXPECT_EQ(1, msg._msg_count);
    EXPECT_EQ("0102C304", msg._source_id);
    EXPECT_EQ(equipment_type::RSU, msg._equipment_type);
    // Confirm timestamp
    EXPECT_EQ(400, msg._time_stamp.offset);
    EXPECT_EQ(5000, msg._time_stamp.second);
    EXPECT_EQ(15, msg._time_stamp.minute);
    EXPECT_EQ(19, msg._time_stamp.hour);
    EXPECT_EQ(4, msg._time_stamp.day);
    EXPECT_EQ(7, msg._time_stamp.month);
    EXPECT_EQ(2007, msg._time_stamp.year);
    // Confirm reference position
    EXPECT_EQ(600000000,msg._ref_positon._longitude);
    EXPECT_EQ(400000000, msg._ref_positon._latitude );
    // Confirm optional elevation is present
    EXPECT_TRUE(msg._ref_positon._elevation.has_value());
    EXPECT_EQ(30, msg._ref_positon._elevation);
    // Confirm positional accuracy
    EXPECT_EQ(235, msg._ref_position_confidence._semi_major_axis_accuracy);
    EXPECT_EQ(200, msg._ref_position_confidence._semi_minor_axis_accuracy);
    EXPECT_EQ(25000, msg._ref_position_confidence._semi_major_axis_orientation);


}

