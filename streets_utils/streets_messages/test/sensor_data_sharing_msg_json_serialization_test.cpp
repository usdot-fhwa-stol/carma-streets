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
#include <serializers/sensor_data_sharing_msg_json_serializer.hpp>
#include <streets_utils/json_utils_lib/json_utils.hpp>

using namespace streets_utils::messages;
using namespace streets_utils::json_utils;

TEST(sensor_data_sharing_msg_json_serialization_test, confirm_required_properties_max_value) {
    sensor_data_sharing_msg msg;
    msg._equipment_type = equipment_type::RSU;
    msg._msg_count = 255;
    msg._source_id = "00000001";
    msg._ref_positon._latitude=900000001;
    msg._ref_positon._longitude=1800000001;
    msg._time_stamp.second = 65535;
    msg._time_stamp.minute = 60;
    msg._time_stamp.hour= 31;
    msg._time_stamp.day = 31;
    msg._time_stamp.month = 12;
    msg._time_stamp.year = 4095; // Max
    msg._ref_position_confidence._semi_major_axis_accuracy = 255;
    msg._ref_position_confidence._semi_minor_axis_accuracy = 255;
    msg._ref_position_confidence._semi_major_axis_orientation = 65535;
    // Add Detected Object
    detected_object_data detected_object;
    detected_object._detected_object_common_data._object_id = 65525;
    detected_object._detected_object_common_data._object_type = object_type::ANIMAL;
    detected_object._detected_object_common_data._classification_confidence = 101;
    detected_object._detected_object_common_data._heading = 28800;
    detected_object._detected_object_common_data._heading_confidence = heading_confidence::PREC_01_deg;
    detected_object._detected_object_common_data._position_offset._offset_x = 32767;
    detected_object._detected_object_common_data._position_offset._offset_y = 32767;
    detected_object._detected_object_common_data._position_offset._offset_z = 32767;
    detected_object._detected_object_common_data._pos_confidence._position_confidence = position_confidence::A_10CM;
    detected_object._detected_object_common_data._pos_confidence._elavation_confidence =  position_confidence::A_1M;
    detected_object._detected_object_common_data._speed = 8191;
    detected_object._detected_object_common_data._time_measurement_offset = 1500;
    detected_object._detected_object_common_data._time_confidence = time_confidence::TIME_000_000_002;
    msg._objects.push_back(detected_object);
    // Serialize Msg
    std::string json = to_json(msg);
    // String is not empty
    EXPECT_FALSE(json.empty());
    // Confirm result has msg information in desired structure
    auto result = parse_json(json);

    // Confirm timestamp data, Should fail at assert statements since if this require property is 
    // not available the other calls will fail
    ASSERT_TRUE(result.HasMember("sdsm_time_stamp"));
    ASSERT_TRUE(result.FindMember("sdsm_time_stamp")->value.IsObject());
    EXPECT_EQ( msg._time_stamp.second, result["sdsm_time_stamp"]["second"].GetInt());
    EXPECT_EQ( msg._time_stamp.minute, result["sdsm_time_stamp"]["minute"].GetInt());
    EXPECT_EQ( msg._time_stamp.hour, result["sdsm_time_stamp"]["hour"].GetInt());
    EXPECT_EQ( msg._time_stamp.day, result["sdsm_time_stamp"]["day"].GetInt());
    EXPECT_EQ( msg._time_stamp.month, result["sdsm_time_stamp"]["month"].GetInt());
    EXPECT_EQ( msg._time_stamp.year, result["sdsm_time_stamp"]["year"].GetInt());

    EXPECT_EQ( msg._msg_count, result["msg_cnt"].GetUint());
    EXPECT_EQ( msg._source_id, result["source_id"].GetString());
    EXPECT_EQ( static_cast<int>(msg._equipment_type), result["equipment_type"].GetInt());
    // Confirm ref position confidence
    ASSERT_TRUE(result.HasMember("ref_pos_xy_conf"));
    ASSERT_TRUE(result.FindMember("ref_pos_xy_conf")->value.IsObject());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_accuracy, result["ref_pos_xy_conf"]["semi_major"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_minor_axis_accuracy, result["ref_pos_xy_conf"]["semi_minor"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_orientation, result["ref_pos_xy_conf"]["orientation"].GetInt());
    // Confirm optional elevation parameter is not present
    EXPECT_FALSE( result.HasMember("ref_pos_el_conf"));
    // Confirm ref position
    ASSERT_TRUE(result.HasMember("ref_pos"));
    ASSERT_TRUE(result.FindMember("ref_pos")->value.IsObject());   
    EXPECT_EQ( msg._ref_positon._longitude, result["ref_pos"]["long"].GetInt64());
    EXPECT_EQ( msg._ref_positon._latitude, result["ref_pos"]["lat"].GetInt64());
    // Optional parameter is not present
    EXPECT_FALSE(result["ref_pos"].HasMember("elevation"));

    // Confirm List of detected object exists
    ASSERT_TRUE(result.HasMember("objects"));
    ASSERT_TRUE(result.FindMember("objects")->value.IsArray());
    ASSERT_EQ(1,result.FindMember("objects")->value.GetArray().Size());
    ASSERT_TRUE(result["objects"].GetArray()[0].IsObject());
    // Confirm object properties
    auto object = result["objects"].GetArray()[0].GetObject();
    // Assert Object has common data
    ASSERT_TRUE(object.HasMember("detected_object_common_data"));
    ASSERT_TRUE(object.FindMember("detected_object_common_data")->value.IsObject());
    ASSERT_FALSE(object.HasMember("detected_object_optional_data"));
    // Retreive Object common data
    auto object_common_data = object["detected_object_common_data"].GetObject();
    // Retreive Object common data from 
    auto msg_object_common_data = msg._objects[0]._detected_object_common_data;
    // Confirm Object common data properties    
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._object_type), object_common_data["obj_type"].GetUint());
    EXPECT_EQ(msg_object_common_data._classification_confidence, object_common_data["obj_type_cfd"].GetUint());
    EXPECT_EQ(msg_object_common_data._object_id, object_common_data["object_id"].GetUint());
    EXPECT_EQ(msg_object_common_data._time_measurement_offset, object_common_data["measurement_time"].GetInt());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._time_confidence), object_common_data["time_confidence"].GetUint());
    EXPECT_EQ(msg_object_common_data._speed, object_common_data["speed"].GetUint());
    EXPECT_EQ(msg_object_common_data._heading, object_common_data["heading"].GetUint());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._heading_confidence), object_common_data["heading_conf"].GetUint());
    // Test Optional properties not present
    EXPECT_FALSE(object_common_data.HasMember("speed_z"));
    EXPECT_FALSE(object_common_data.HasMember("speed_confidence_z"));
    EXPECT_FALSE(object_common_data.HasMember("accel_4_way"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_x"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_y"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_yaw"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_z"));

}

TEST(sensor_data_sharing_msg_json_serialization_test, confirm_required_properties_min_value) {
    sensor_data_sharing_msg msg;
    msg._equipment_type = equipment_type::OBU;
    msg._msg_count = 1;
    msg._source_id = "00000001";
    msg._ref_positon._latitude=-90000000;
    msg._ref_positon._longitude=-1800000000;
    msg._time_stamp.second = 0;
    msg._time_stamp.minute = 0;
    msg._time_stamp.hour= 0;
    msg._time_stamp.day = 0;
    msg._time_stamp.month = 0;
    msg._time_stamp.year = 0; 
    msg._ref_position_confidence._semi_major_axis_accuracy = 0;
    msg._ref_position_confidence._semi_minor_axis_accuracy = 0;
    msg._ref_position_confidence._semi_major_axis_orientation = 0;
    // Add Detected Object
    detected_object_data detected_object;
    detected_object._detected_object_common_data._object_id = 0;
    detected_object._detected_object_common_data._object_type = object_type::VEHICLE;
    detected_object._detected_object_common_data._classification_confidence = 0;
    detected_object._detected_object_common_data._heading = 0;
    detected_object._detected_object_common_data._heading_confidence = heading_confidence::PREC_0_0125_deg;
    detected_object._detected_object_common_data._position_offset._offset_x = -32767;
    detected_object._detected_object_common_data._position_offset._offset_y = -32767;
    detected_object._detected_object_common_data._position_offset._offset_z = -32767;
    detected_object._detected_object_common_data._pos_confidence._position_confidence = position_confidence::A_500M;
    detected_object._detected_object_common_data._pos_confidence._elavation_confidence =  position_confidence::A_1CM;
    detected_object._detected_object_common_data._speed = 0;
    detected_object._detected_object_common_data._time_measurement_offset = -1500;
    detected_object._detected_object_common_data._time_confidence = time_confidence::TIME_000_000_000_000_01;
    msg._objects.push_back(detected_object);
    // Serialize Msg
    std::string json = to_json(msg);
    // String is not empty
    EXPECT_FALSE(json.empty());
    // Confirm result has msg information in desired structure
    auto result = parse_json(json);

    // Confirm timestamp data, Should fail at assert statements since if this require property is 
    // not available the other calls will fail
    ASSERT_TRUE(result.HasMember("sdsm_time_stamp"));
    ASSERT_TRUE(result.FindMember("sdsm_time_stamp")->value.IsObject());
    EXPECT_EQ( msg._time_stamp.second, result["sdsm_time_stamp"]["second"].GetInt());
    EXPECT_EQ( msg._time_stamp.minute, result["sdsm_time_stamp"]["minute"].GetInt());
    EXPECT_EQ( msg._time_stamp.hour, result["sdsm_time_stamp"]["hour"].GetInt());
    EXPECT_EQ( msg._time_stamp.day, result["sdsm_time_stamp"]["day"].GetInt());
    EXPECT_EQ( msg._time_stamp.month, result["sdsm_time_stamp"]["month"].GetInt());
    EXPECT_EQ( msg._time_stamp.year, result["sdsm_time_stamp"]["year"].GetInt());

    EXPECT_EQ( msg._msg_count, result["msg_cnt"].GetUint());
    EXPECT_EQ( msg._source_id, result["source_id"].GetString());
    EXPECT_EQ( static_cast<int>(msg._equipment_type), result["equipment_type"].GetInt());
    // Confirm ref position confidence
    ASSERT_TRUE(result.HasMember("ref_pos_xy_conf"));
    ASSERT_TRUE(result.FindMember("ref_pos_xy_conf")->value.IsObject());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_accuracy, result["ref_pos_xy_conf"]["semi_major"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_minor_axis_accuracy, result["ref_pos_xy_conf"]["semi_minor"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_orientation, result["ref_pos_xy_conf"]["orientation"].GetInt());
    // Confirm optional elevation parameter is not present
    EXPECT_FALSE( result.HasMember("ref_pos_el_conf"));
    // Confirm ref position
    ASSERT_TRUE(result.HasMember("ref_pos"));
    ASSERT_TRUE(result.FindMember("ref_pos")->value.IsObject());   
    EXPECT_EQ( msg._ref_positon._longitude, result["ref_pos"]["long"].GetInt64());
    EXPECT_EQ( msg._ref_positon._latitude, result["ref_pos"]["lat"].GetInt64());
    // Optional parameter is not present
    EXPECT_FALSE(result["ref_pos"].HasMember("elevation"));

    // Confirm List of detected object exists
    ASSERT_TRUE(result.HasMember("objects"));
    ASSERT_TRUE(result.FindMember("objects")->value.IsArray());
    ASSERT_EQ(1,result.FindMember("objects")->value.GetArray().Size());
    ASSERT_TRUE(result["objects"].GetArray()[0].IsObject());
    // Confirm object properties
    auto object = result["objects"].GetArray()[0].GetObject();
    // Assert Object has common data
    ASSERT_TRUE(object.HasMember("detected_object_common_data"));
    ASSERT_TRUE(object.FindMember("detected_object_common_data")->value.IsObject());
    ASSERT_FALSE(object.HasMember("detected_object_optional_data"));
    // Retreive Object common data
    auto object_common_data = object["detected_object_common_data"].GetObject();
    // Retreive Object common data from 
    auto msg_object_common_data = msg._objects[0]._detected_object_common_data;
    // Confirm Object common data properties    
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._object_type), object_common_data["obj_type"].GetUint());
    EXPECT_EQ(msg_object_common_data._classification_confidence, object_common_data["obj_type_cfd"].GetUint());
    EXPECT_EQ(msg_object_common_data._object_id, object_common_data["object_id"].GetUint());
    EXPECT_EQ(msg_object_common_data._time_measurement_offset, object_common_data["measurement_time"].GetInt());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._time_confidence), object_common_data["time_confidence"].GetUint());
    EXPECT_EQ(msg_object_common_data._speed, object_common_data["speed"].GetUint());
    EXPECT_EQ(msg_object_common_data._heading, object_common_data["heading"].GetUint());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._heading_confidence), object_common_data["heading_conf"].GetUint());
    // Test Optional properties not present
    EXPECT_FALSE(object_common_data.HasMember("speed_z"));
    EXPECT_FALSE(object_common_data.HasMember("speed_confidence_z"));
    EXPECT_FALSE(object_common_data.HasMember("accel_4_way"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_x"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_y"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_yaw"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_z"));

}

TEST(sensor_data_sharing_msg_json_serialization_test, confirm_optional_vru_properties) {
    sensor_data_sharing_msg msg;
    msg._equipment_type = equipment_type::OBU;
    msg._msg_count = 1;
    msg._source_id = "00000001";
    msg._ref_positon._latitude=-90000000;
    msg._ref_positon._longitude=-1800000000;
    msg._ref_positon._elavation = -32768;
    msg._time_stamp.second = 0;
    msg._time_stamp.minute = 0;
    msg._time_stamp.hour= 0;
    msg._time_stamp.day = 0;
    msg._time_stamp.month = 0;
    msg._time_stamp.year = 0; 
    msg._ref_position_confidence._semi_major_axis_accuracy = 0;
    msg._ref_position_confidence._semi_minor_axis_accuracy = 0;
    msg._ref_position_confidence._semi_major_axis_orientation = 0;
    msg._ref_position_elavation_confidence = position_confidence::A_10M;
    // Add Detected Object
    detected_object_data detected_object;
    detected_object._detected_object_common_data._object_id = 0;
    detected_object._detected_object_common_data._object_type = object_type::VEHICLE;
    detected_object._detected_object_common_data._classification_confidence = 0;
    detected_object._detected_object_common_data._heading = 0;
    detected_object._detected_object_common_data._heading_confidence = heading_confidence::PREC_0_0125_deg;
    detected_object._detected_object_common_data._position_offset._offset_x = -32767;
    detected_object._detected_object_common_data._position_offset._offset_y = -32767;
    detected_object._detected_object_common_data._position_offset._offset_z = -32767;
    detected_object._detected_object_common_data._pos_confidence._position_confidence = position_confidence::A_500M;
    detected_object._detected_object_common_data._pos_confidence._elavation_confidence =  position_confidence::A_1CM;
    detected_object._detected_object_common_data._speed = 0;
    detected_object._detected_object_common_data._speed_z = 8191;
    detected_object._detected_object_common_data._speed_confidence = speed_confidence::UNAVAILABLE;
    detected_object._detected_object_common_data._speed_z_confidence = speed_confidence::PREC_100ms;
    // Create acceleration 4 way
    acceleration_set_4_way accel_set;
    accel_set._lateral_accel = 2001;
    accel_set._longitudinal_accel =2001;
    accel_set._vertical_accel = 127;
    accel_set._yaw_rate = 32767;
    detected_object._detected_object_common_data._acceleration_4_way= accel_set;
    // Create acceleration confidence set
    acceleration_confidence_set_4_way accel_set_confidence;
    accel_set_confidence._lateral_confidence = acceleration_confidence::ACCL_0_1;
    accel_set_confidence._longitudinal_confidence = acceleration_confidence::ACCL_100;
    accel_set_confidence._vertical_confidence = acceleration_confidence::ACCL_0_01;
    accel_set_confidence._yaw_rate_confidence = angular_velocity_confidence::UNAVAILABLE;
    detected_object._detected_object_common_data._acceleration_confidence = accel_set_confidence;
    
    detected_object._detected_object_common_data._time_measurement_offset = -1500;
    detected_object._detected_object_common_data._time_confidence = time_confidence::TIME_000_000_000_000_01;
    // Add Detected VRU Optional data
    detected_vru_data detected_vru;
    detected_vru._attachment = attachment::WHEEL_CHAIR;
    detected_vru._personal_device_user_type = personal_device_user_type::PEDESTRIAN;
    detected_vru._attachment_radius = 200;
    detected_vru._propulsion = human_propelled_type::WHEELCHAIR;

    detected_object._detected_object_optional_data = detected_vru;
    msg._objects.push_back(detected_object);
    // Serialize Msg
    std::string json = to_json(msg);
    // String is not empty
    EXPECT_FALSE(json.empty());
    // Confirm result has msg information in desired structure
    auto result = parse_json(json);

    // Confirm timestamp data, Should fail at assert statements since if this require property is 
    // not available the other calls will fail
    ASSERT_TRUE(result.HasMember("sdsm_time_stamp"));
    ASSERT_TRUE(result.FindMember("sdsm_time_stamp")->value.IsObject());
    EXPECT_EQ( msg._time_stamp.second, result["sdsm_time_stamp"]["second"].GetInt());
    EXPECT_EQ( msg._time_stamp.minute, result["sdsm_time_stamp"]["minute"].GetInt());
    EXPECT_EQ( msg._time_stamp.hour, result["sdsm_time_stamp"]["hour"].GetInt());
    EXPECT_EQ( msg._time_stamp.day, result["sdsm_time_stamp"]["day"].GetInt());
    EXPECT_EQ( msg._time_stamp.month, result["sdsm_time_stamp"]["month"].GetInt());
    EXPECT_EQ( msg._time_stamp.year, result["sdsm_time_stamp"]["year"].GetInt());

    EXPECT_EQ( msg._msg_count, result["msg_cnt"].GetUint());
    EXPECT_EQ( msg._source_id, result["source_id"].GetString());
    EXPECT_EQ( static_cast<int>(msg._equipment_type), result["equipment_type"].GetInt());
    // Confirm ref position confidence
    ASSERT_TRUE(result.HasMember("ref_pos_xy_conf"));
    ASSERT_TRUE(result.FindMember("ref_pos_xy_conf")->value.IsObject());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_accuracy, result["ref_pos_xy_conf"]["semi_major"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_minor_axis_accuracy, result["ref_pos_xy_conf"]["semi_minor"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_orientation, result["ref_pos_xy_conf"]["orientation"].GetInt());
    // Confirm optional elevation parameter is present
    EXPECT_TRUE( result.HasMember("ref_pos_el_conf"));
    // Confirm ref position
    ASSERT_TRUE(result.HasMember("ref_pos"));
    ASSERT_TRUE(result.FindMember("ref_pos")->value.IsObject());   
    EXPECT_EQ( msg._ref_positon._longitude, result["ref_pos"]["long"].GetInt64());
    EXPECT_EQ( msg._ref_positon._latitude, result["ref_pos"]["lat"].GetInt64());
    // Optional parameter is present
    EXPECT_TRUE(result["ref_pos"].HasMember("elevation"));

    // Confirm List of detected object exists
    ASSERT_TRUE(result.HasMember("objects"));
    ASSERT_TRUE(result.FindMember("objects")->value.IsArray());
    ASSERT_EQ(1,result.FindMember("objects")->value.GetArray().Size());
    ASSERT_TRUE(result["objects"].GetArray()[0].IsObject());
    // Confirm object properties
    auto object = result["objects"].GetArray()[0].GetObject();
    // Assert Object has common data
    ASSERT_TRUE(object.HasMember("detected_object_common_data"));
    ASSERT_TRUE(object.FindMember("detected_object_common_data")->value.IsObject());
    // Retreive Object common data
    auto object_common_data = object["detected_object_common_data"].GetObject();
    // Retreive Object common data from 
    auto msg_object_common_data = msg._objects[0]._detected_object_common_data;
    // Confirm Object common data properties    
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._object_type), object_common_data["obj_type"].GetUint());
    EXPECT_EQ(msg_object_common_data._classification_confidence, object_common_data["obj_type_cfd"].GetUint());
    EXPECT_EQ(msg_object_common_data._object_id, object_common_data["object_id"].GetUint());
    EXPECT_EQ(msg_object_common_data._time_measurement_offset, object_common_data["measurement_time"].GetInt());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._time_confidence), object_common_data["time_confidence"].GetUint());
    EXPECT_EQ(msg_object_common_data._speed, object_common_data["speed"].GetUint());
    EXPECT_EQ(msg_object_common_data._speed_confidence, speed_confidence_from_int (object_common_data["speed_confidence"].GetUint()));
    EXPECT_EQ(msg_object_common_data._heading, object_common_data["heading"].GetUint());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._heading_confidence), object_common_data["heading_conf"].GetUint());
    // Test Optional properties not present
    EXPECT_EQ(msg_object_common_data._speed_z, object_common_data["speed_z"].GetUint());
    EXPECT_EQ(msg_object_common_data._speed_z_confidence, speed_confidence_from_int(object_common_data["speed_confidence_z"].GetUint()) );
    EXPECT_TRUE(object_common_data.HasMember("accel_4_way"));
    EXPECT_TRUE(object_common_data.HasMember("acc_cfd_x"));
    EXPECT_TRUE(object_common_data.HasMember("acc_cfd_y"));
    EXPECT_TRUE(object_common_data.HasMember("acc_cfd_yaw"));
    EXPECT_TRUE(object_common_data.HasMember("acc_cfd_z"));
    // TODO: expect vru optional fields information
    ASSERT_TRUE(object.HasMember("detected_object_optional_data"));

}


TEST(sensor_data_sharing_msg_json_serialization_test, confirm_optional_obstacle_properties) {
    sensor_data_sharing_msg msg;
    msg._equipment_type = equipment_type::OBU;
    msg._msg_count = 1;
    msg._source_id = "00000001";
    msg._ref_positon._latitude=-90000000;
    msg._ref_positon._longitude=-1800000000;
    msg._time_stamp.second = 0;
    msg._time_stamp.minute = 0;
    msg._time_stamp.hour= 0;
    msg._time_stamp.day = 0;
    msg._time_stamp.month = 0;
    msg._time_stamp.year = 0; 
    msg._ref_position_confidence._semi_major_axis_accuracy = 0;
    msg._ref_position_confidence._semi_minor_axis_accuracy = 0;
    msg._ref_position_confidence._semi_major_axis_orientation = 0;
    // Add Detected Object
    detected_object_data detected_object;
    detected_object._detected_object_common_data._object_id = 0;
    detected_object._detected_object_common_data._object_type = object_type::VEHICLE;
    detected_object._detected_object_common_data._classification_confidence = 0;
    detected_object._detected_object_common_data._heading = 0;
    detected_object._detected_object_common_data._heading_confidence = heading_confidence::PREC_0_0125_deg;
    detected_object._detected_object_common_data._position_offset._offset_x = -32767;
    detected_object._detected_object_common_data._position_offset._offset_y = -32767;
    detected_object._detected_object_common_data._position_offset._offset_z = -32767;
    detected_object._detected_object_common_data._pos_confidence._position_confidence = position_confidence::A_500M;
    detected_object._detected_object_common_data._pos_confidence._elavation_confidence =  position_confidence::A_1CM;
    detected_object._detected_object_common_data._speed = 0;
    detected_object._detected_object_common_data._time_measurement_offset = -1500;
    detected_object._detected_object_common_data._time_confidence = time_confidence::TIME_000_000_000_000_01;
    // Add Detected VRU Optional data
    detected_obstacle_data detected_obstacle;
    detected_obstacle._size._height = 1023;
    detected_obstacle._size._width = 1023;
    detected_obstacle._size._length = 1023;
    
    detected_obstacle._size_confidence._height_confidence = size_value_confidence::SIZE_0_01;
    detected_obstacle._size_confidence._width_confidence = size_value_confidence::SIZE_0_01;
    detected_obstacle._size_confidence._length_confidence = size_value_confidence::SIZE_0_01;


    detected_object._detected_object_optional_data = detected_obstacle;
    msg._objects.push_back(detected_object);
    // Serialize Msg
    std::string json = to_json(msg);
    // String is not empty
    EXPECT_FALSE(json.empty());
    // Confirm result has msg information in desired structure
    auto result = parse_json(json);

    // Confirm timestamp data, Should fail at assert statements since if this require property is 
    // not available the other calls will fail
    ASSERT_TRUE(result.HasMember("sdsm_time_stamp"));
    ASSERT_TRUE(result.FindMember("sdsm_time_stamp")->value.IsObject());
    EXPECT_EQ( msg._time_stamp.second, result["sdsm_time_stamp"]["second"].GetInt());
    EXPECT_EQ( msg._time_stamp.minute, result["sdsm_time_stamp"]["minute"].GetInt());
    EXPECT_EQ( msg._time_stamp.hour, result["sdsm_time_stamp"]["hour"].GetInt());
    EXPECT_EQ( msg._time_stamp.day, result["sdsm_time_stamp"]["day"].GetInt());
    EXPECT_EQ( msg._time_stamp.month, result["sdsm_time_stamp"]["month"].GetInt());
    EXPECT_EQ( msg._time_stamp.year, result["sdsm_time_stamp"]["year"].GetInt());

    EXPECT_EQ( msg._msg_count, result["msg_cnt"].GetUint());
    EXPECT_EQ( msg._source_id, result["source_id"].GetString());
    EXPECT_EQ( static_cast<int>(msg._equipment_type), result["equipment_type"].GetInt());
    // Confirm ref position confidence
    ASSERT_TRUE(result.HasMember("ref_pos_xy_conf"));
    ASSERT_TRUE(result.FindMember("ref_pos_xy_conf")->value.IsObject());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_accuracy, result["ref_pos_xy_conf"]["semi_major"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_minor_axis_accuracy, result["ref_pos_xy_conf"]["semi_minor"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_orientation, result["ref_pos_xy_conf"]["orientation"].GetInt());
    // Confirm optional elevation parameter is not present
    EXPECT_FALSE( result.HasMember("ref_pos_el_conf"));
    // Confirm ref position
    ASSERT_TRUE(result.HasMember("ref_pos"));
    ASSERT_TRUE(result.FindMember("ref_pos")->value.IsObject());   
    EXPECT_EQ( msg._ref_positon._longitude, result["ref_pos"]["long"].GetInt64());
    EXPECT_EQ( msg._ref_positon._latitude, result["ref_pos"]["lat"].GetInt64());
    // Optional parameter is not present
    EXPECT_FALSE(result["ref_pos"].HasMember("elevation"));

    // Confirm List of detected object exists
    ASSERT_TRUE(result.HasMember("objects"));
    ASSERT_TRUE(result.FindMember("objects")->value.IsArray());
    ASSERT_EQ(1,result.FindMember("objects")->value.GetArray().Size());
    ASSERT_TRUE(result["objects"].GetArray()[0].IsObject());
    // Confirm object properties
    auto object = result["objects"].GetArray()[0].GetObject();
    // Assert Object has common data
    ASSERT_TRUE(object.HasMember("detected_object_common_data"));
    ASSERT_TRUE(object.FindMember("detected_object_common_data")->value.IsObject());
    // Retreive Object common data
    auto object_common_data = object["detected_object_common_data"].GetObject();
    // Retreive Object common data from 
    auto msg_object_common_data = msg._objects[0]._detected_object_common_data;
    // Confirm Object common data properties    
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._object_type), object_common_data["obj_type"].GetUint());
    EXPECT_EQ(msg_object_common_data._classification_confidence, object_common_data["obj_type_cfd"].GetUint());
    EXPECT_EQ(msg_object_common_data._object_id, object_common_data["object_id"].GetUint());
    EXPECT_EQ(msg_object_common_data._time_measurement_offset, object_common_data["measurement_time"].GetInt());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._time_confidence), object_common_data["time_confidence"].GetUint());
    EXPECT_EQ(msg_object_common_data._speed, object_common_data["speed"].GetUint());
    EXPECT_EQ(msg_object_common_data._heading, object_common_data["heading"].GetUint());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._heading_confidence), object_common_data["heading_conf"].GetUint());
    // Test Optional properties not present
    EXPECT_FALSE(object_common_data.HasMember("speed_z"));
    EXPECT_FALSE(object_common_data.HasMember("speed_confidence_z"));
    EXPECT_FALSE(object_common_data.HasMember("accel_4_way"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_x"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_y"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_yaw"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_z"));
    // TODO: expect vru optional fields information
    ASSERT_TRUE(object.HasMember("detected_object_optional_data"));

}

TEST(sensor_data_sharing_msg_json_serialization_test, confirm_optional_vehicle_properties) {
    sensor_data_sharing_msg msg;
    msg._equipment_type = equipment_type::OBU;
    msg._msg_count = 1;
    msg._source_id = "00000001";
    msg._ref_positon._latitude=-90000000;
    msg._ref_positon._longitude=-1800000000;
    msg._time_stamp.second = 0;
    msg._time_stamp.minute = 0;
    msg._time_stamp.hour= 0;
    msg._time_stamp.day = 0;
    msg._time_stamp.month = 0;
    msg._time_stamp.year = 0; 
    msg._ref_position_confidence._semi_major_axis_accuracy = 0;
    msg._ref_position_confidence._semi_minor_axis_accuracy = 0;
    msg._ref_position_confidence._semi_major_axis_orientation = 0;
    // Add Detected Object
    detected_object_data detected_object;
    detected_object._detected_object_common_data._object_id = 0;
    detected_object._detected_object_common_data._object_type = object_type::VEHICLE;
    detected_object._detected_object_common_data._classification_confidence = 0;
    detected_object._detected_object_common_data._heading = 0;
    detected_object._detected_object_common_data._heading_confidence = heading_confidence::PREC_0_0125_deg;
    detected_object._detected_object_common_data._position_offset._offset_x = -32767;
    detected_object._detected_object_common_data._position_offset._offset_y = -32767;
    detected_object._detected_object_common_data._position_offset._offset_z = -32767;
    detected_object._detected_object_common_data._pos_confidence._position_confidence = position_confidence::A_500M;
    detected_object._detected_object_common_data._pos_confidence._elavation_confidence =  position_confidence::A_1CM;
    detected_object._detected_object_common_data._speed = 0;
    detected_object._detected_object_common_data._time_measurement_offset = -1500;
    detected_object._detected_object_common_data._time_confidence = time_confidence::TIME_000_000_000_000_01;
    // Add detected vehicle optional data
    detected_vehicle_data detected_vehicle;
    detected_vehicle._size->_width = 4095;
    detected_vehicle._size->_length = 4095;
    detected_vehicle._vehicle_height = 127;

    detected_vehicle._size_confidence->_height_confidence = size_value_confidence::SIZE_0_01;
    detected_vehicle._size_confidence->_width_confidence = size_value_confidence::SIZE_0_01;
    detected_vehicle._size_confidence->_length_confidence = size_value_confidence::SIZE_0_01;

    detected_vehicle.exterior_lights = "11110000";
    
    detected_vehicle._angular_velocity->_pitch_rate = 32767;
    detected_vehicle._angular_velocity->_roll_rate = 32767;
    
    detected_vehicle._angular_velocity_confidence->_pitch_rate_confidence = angular_velocity_confidence::DEGSEC_01;
    detected_vehicle._angular_velocity_confidence->_roll_rate_confidence = angular_velocity_confidence::DEGSEC_01;
    
    detected_vehicle._veh_attitude->_pitch = 72000;
    detected_vehicle._veh_attitude->_roll = 14400;
    detected_vehicle._veh_attitude->_yaw = 14400;

    detected_vehicle._attitude_confidence->_pitch_confidence = heading_confidence::PREC_0_05_deg;
    detected_vehicle._attitude_confidence->_roll_confidence = heading_confidence::PREC_10_deg;
    detected_vehicle._attitude_confidence->_yaw_confidence = heading_confidence::PREC_01_deg;

    detected_vehicle._vehicle_class = 23;
    detected_vehicle._classification_confidence = 101;

    detected_object._detected_object_optional_data = detected_vehicle;
    msg._objects.push_back(detected_object);
    // Serialize Msg
    std::string json = to_json(msg);
    // String is not empty
    EXPECT_FALSE(json.empty());
    // Confirm result has msg information in desired structure
    auto result = parse_json(json);

    // Confirm timestamp data, Should fail at assert statements since if this require property is 
    // not available the other calls will fail
    ASSERT_TRUE(result.HasMember("sdsm_time_stamp"));
    ASSERT_TRUE(result.FindMember("sdsm_time_stamp")->value.IsObject());
    EXPECT_EQ( msg._time_stamp.second, result["sdsm_time_stamp"]["second"].GetInt());
    EXPECT_EQ( msg._time_stamp.minute, result["sdsm_time_stamp"]["minute"].GetInt());
    EXPECT_EQ( msg._time_stamp.hour, result["sdsm_time_stamp"]["hour"].GetInt());
    EXPECT_EQ( msg._time_stamp.day, result["sdsm_time_stamp"]["day"].GetInt());
    EXPECT_EQ( msg._time_stamp.month, result["sdsm_time_stamp"]["month"].GetInt());
    EXPECT_EQ( msg._time_stamp.year, result["sdsm_time_stamp"]["year"].GetInt());

    EXPECT_EQ( msg._msg_count, result["msg_cnt"].GetUint());
    EXPECT_EQ( msg._source_id, result["source_id"].GetString());
    EXPECT_EQ( static_cast<int>(msg._equipment_type), result["equipment_type"].GetInt());
    // Confirm ref position confidence
    ASSERT_TRUE(result.HasMember("ref_pos_xy_conf"));
    ASSERT_TRUE(result.FindMember("ref_pos_xy_conf")->value.IsObject());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_accuracy, result["ref_pos_xy_conf"]["semi_major"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_minor_axis_accuracy, result["ref_pos_xy_conf"]["semi_minor"].GetUint());
    EXPECT_EQ(msg._ref_position_confidence._semi_major_axis_orientation, result["ref_pos_xy_conf"]["orientation"].GetInt());
    // Confirm optional elevation parameter is not present
    EXPECT_FALSE( result.HasMember("ref_pos_el_conf"));
    // Confirm ref position
    ASSERT_TRUE(result.HasMember("ref_pos"));
    ASSERT_TRUE(result.FindMember("ref_pos")->value.IsObject());   
    EXPECT_EQ( msg._ref_positon._longitude, result["ref_pos"]["long"].GetInt64());
    EXPECT_EQ( msg._ref_positon._latitude, result["ref_pos"]["lat"].GetInt64());
    // Optional parameter is not present
    EXPECT_FALSE(result["ref_pos"].HasMember("elevation"));

    // Confirm List of detected object exists
    ASSERT_TRUE(result.HasMember("objects"));
    ASSERT_TRUE(result.FindMember("objects")->value.IsArray());
    ASSERT_EQ(1,result.FindMember("objects")->value.GetArray().Size());
    ASSERT_TRUE(result["objects"].GetArray()[0].IsObject());
    // Confirm object properties
    auto object = result["objects"].GetArray()[0].GetObject();
    // Assert Object has common data
    ASSERT_TRUE(object.HasMember("detected_object_common_data"));
    ASSERT_TRUE(object.FindMember("detected_object_common_data")->value.IsObject());
    // Retreive Object common data
    auto object_common_data = object["detected_object_common_data"].GetObject();
    // Retreive Object common data from 
    auto msg_object_common_data = msg._objects[0]._detected_object_common_data;
    // Confirm Object common data properties    
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._object_type), object_common_data["obj_type"].GetUint());
    EXPECT_EQ(msg_object_common_data._classification_confidence, object_common_data["obj_type_cfd"].GetUint());
    EXPECT_EQ(msg_object_common_data._object_id, object_common_data["object_id"].GetUint());
    EXPECT_EQ(msg_object_common_data._time_measurement_offset, object_common_data["measurement_time"].GetInt());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._time_confidence), object_common_data["time_confidence"].GetUint());
    EXPECT_EQ(msg_object_common_data._speed, object_common_data["speed"].GetUint());
    EXPECT_EQ(msg_object_common_data._heading, object_common_data["heading"].GetUint());
    EXPECT_EQ(static_cast<unsigned int >(msg_object_common_data._heading_confidence), object_common_data["heading_conf"].GetUint());
    // Test Optional properties not present
    EXPECT_FALSE(object_common_data.HasMember("speed_z"));
    EXPECT_FALSE(object_common_data.HasMember("speed_confidence_z"));
    EXPECT_FALSE(object_common_data.HasMember("accel_4_way"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_x"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_y"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_yaw"));
    EXPECT_FALSE(object_common_data.HasMember("acc_cfd_z"));
    // TODO: expect vru optional fields information
    ASSERT_TRUE(object.HasMember("detected_object_optional_data"));

}

