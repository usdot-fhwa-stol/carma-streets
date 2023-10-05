#include <gtest/gtest.h>
#include <serializers/sensor_data_sharing_msg_json_serializer.hpp>
#include "json_utils.hpp"

using namespace streets_utils::messages;
using namespace streets_utils::json_utils;

TEST(sensor_data_sharing_msg_json_serialization_test, confirm_required_components) {
    sensor_data_sharing_msg msg;
    msg._equipment_type = equipment_type::RSU;
    msg._msg_count = 1;
    msg._source_id = "rsu_1";
    msg._ref_positon._latitude=0;
    msg._ref_positon._longitude=0;
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
    detected_object._detected_object_common_data._classification_confidence = 2;
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

    EXPECT_EQ( msg._msg_count, result["msg_count"]);
    // EXPECT_EQ( );
}