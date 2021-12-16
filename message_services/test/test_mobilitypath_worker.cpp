#include <rapidjson/document.h>

#include "gtest/gtest.h"
#include "mobilitypath_worker.h"

TEST(test_mobilitypath_worker, get_curr_map)
{
    message_services::workers::mobilitypath_worker mobilitypath_w_obj;
    ASSERT_EQ(0, mobilitypath_w_obj.get_curr_map().size());
}

TEST(test_mobilitypath_worker, process_incoming_msg)
{
    message_services::workers::mobilitypath_worker mobilitypath_w_obj;
    ASSERT_EQ(0, mobilitypath_w_obj.get_curr_map().size());
    std::string mobilitypath_json_str = "{\"metadata\": {\"timestamp\" : \"1632679657\",\"hostStaticId\": \"DOT-507\",\"hostBSMId\": \"bsmXXXid\"}, \"trajectory\": { \"location\": {\"ecefX\": 122, \"ecef_y\": 1,\"ecefZ\": 0}}}";
    mobilitypath_w_obj.process_incoming_msg(mobilitypath_json_str);
    ASSERT_EQ(1632679657, mobilitypath_w_obj.get_curr_map().begin()->second.getHeader().timestamp);
    ASSERT_EQ("DOT-507", mobilitypath_w_obj.get_curr_map().begin()->second.getHeader().sender_id);
    ASSERT_EQ("bsmXXXid", mobilitypath_w_obj.get_curr_map().begin()->second.getHeader().sender_bsm_id);
    ASSERT_EQ(1, mobilitypath_w_obj.get_curr_map().size());
}

TEST(test_mobilitypath_worker, pop_cur_element_from_list)
{
    message_services::workers::mobilitypath_worker mobilitypath_w_obj;
    ASSERT_EQ(0, mobilitypath_w_obj.get_curr_map().size());
    std::string mobilitypath_json_str_1 = "{\"metadata\": {\"timestamp\" : \"1632679657\",\"hostStaticId\": \"DOT-507\",\"hostBSMId\": \"bsmXXXid\"}, \"trajectory\": { \"location\": {\"ecefX\": 122, \"ecefY\": 1,\"ecefZ\": 0,\"timestamp\": \"1632679657\"}}}";
    std::string mobilitypath_json_str_2 = "{\"metadata\": {\"timestamp\" : \"1632679758\",\"hostStaticId\": \"DOT-508\",\"planId\" : \"00000000-0000-0000-0000-000000000000\",\"hostBSMId\": \"bsmXXXid\"}, \"trajectory\": { \"location\": {\"ecefX\": 122, \"ecefY\": 1,\"ecefZ\": 0,\"timestamp\": \"1632679657\"}, \"offsets\": [{\"offsetX\": 122, \"offsetY\": 1,\"offsetZ\": 0}] }}";
    std::string mobilitypath_json_str_3 = "{\"metadata\": {\"timestamp\" : \"1632679859\",\"hostStaticId\": \"DOT-509\" ,\"planId\" : \"00000000-0000-0000-0000-000000000000\",\"hostBSMId\": \"bsmXXXid\"}, \"trajectory\": { \"location\": {\"ecefX\": 122, \"ecefY\": 1,\"ecefZ\": 0,\"timestamp\": \"1632679657\"}, \"offsets\": [{\"offsetX\": 122, \"offsetY\": 1,\"offsetZ\": 0}]}}";
    mobilitypath_w_obj.process_incoming_msg(mobilitypath_json_str_1);
    mobilitypath_w_obj.process_incoming_msg(mobilitypath_json_str_2);
    mobilitypath_w_obj.process_incoming_msg(mobilitypath_json_str_3);
    ASSERT_EQ(3, mobilitypath_w_obj.get_curr_map().size());
    mobilitypath_w_obj.get_curr_map().erase("DOT-50716326796");
    ASSERT_EQ(2, mobilitypath_w_obj.get_curr_map().size());
    // ASSERT_EQ("00000000-0000-0000-0000-000000000000", mobilitypath_w_obj.get_curr_map().find("DOT-50716326798")->second.getHeader().plan_id);
    ASSERT_EQ("DOT-508", mobilitypath_w_obj.get_curr_map().begin()->second.getHeader().sender_id);
    ASSERT_EQ("bsmXXXid", mobilitypath_w_obj.get_curr_map().begin()->second.getHeader().sender_bsm_id);
    ASSERT_EQ("", mobilitypath_w_obj.get_curr_map().begin()->second.getHeader().recipient_id);
    ASSERT_EQ(1632679758, mobilitypath_w_obj.get_curr_map().begin()->second.getHeader().timestamp);
    std::cout << mobilitypath_w_obj.get_curr_map().begin()->second.getTrajectory().offsets.front().offset_x;
    ASSERT_EQ(122, mobilitypath_w_obj.get_curr_map().begin()->second.getTrajectory().offsets.begin()->offset_x);
    ASSERT_EQ(1, mobilitypath_w_obj.get_curr_map().begin()->second.getTrajectory().offsets.begin()->offset_y);
    ASSERT_EQ(0, mobilitypath_w_obj.get_curr_map().begin()->second.getTrajectory().offsets.begin()->offset_z);
    ASSERT_EQ(122, mobilitypath_w_obj.get_curr_map().begin()->second.getTrajectory().location.ecef_x);
    ASSERT_EQ(1, mobilitypath_w_obj.get_curr_map().begin()->second.getTrajectory().location.ecef_y);
    ASSERT_EQ(0, mobilitypath_w_obj.get_curr_map().begin()->second.getTrajectory().location.ecef_z);
    ASSERT_EQ(1632679657, mobilitypath_w_obj.get_curr_map().begin()->second.getTrajectory().location.timestamp);
}