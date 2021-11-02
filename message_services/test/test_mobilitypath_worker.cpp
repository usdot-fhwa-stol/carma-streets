#include <rapidjson/document.h>

#include "gtest/gtest.h"
#include "mobilitypath_worker.h"

TEST(test_mobilitypath_worker, get_curr_list)
{
    message_services::workers::mobilitypath_worker mobilitypath_w_obj;
    ASSERT_EQ(0, mobilitypath_w_obj.get_curr_list().size());
}

TEST(test_mobilitypath_worker, process_incoming_msg)
{
    message_services::workers::mobilitypath_worker mobilitypath_w_obj;
    ASSERT_EQ(0, mobilitypath_w_obj.get_curr_list().size());
    std::string mobilitypath_json_str = "{\"metadata\": {\"timestamp\" : \"1632679657\",\"hostStaticId\": \"DOT-507\",\"hostBSMId\": \"bsmXXXid\"}, \"trajectory\": { \"location\": {\"ecef_x\": 122, \"ecef_y\": 1,\"ecef_z\": 0}}}";
    mobilitypath_w_obj.process_incoming_msg(mobilitypath_json_str);
    ASSERT_EQ(1632679657, mobilitypath_w_obj.get_curr_list().front().getHeader().timestamp);
    ASSERT_EQ("DOT-507", mobilitypath_w_obj.get_curr_list().front().getHeader().sender_id);
    ASSERT_EQ("bsmXXXid", mobilitypath_w_obj.get_curr_list().front().getHeader().sender_bsm_id);
    ASSERT_EQ(1, mobilitypath_w_obj.get_curr_list().size());
}

TEST(test_mobilitypath_worker, pop_cur_element_from_list)
{
    message_services::workers::mobilitypath_worker mobilitypath_w_obj;
    ASSERT_EQ(0, mobilitypath_w_obj.get_curr_list().size());
    std::string mobilitypath_json_str_1 = "{\"metadata\": {\"timestamp\" : \"1632679657\",\"hostStaticId\": \"DOT-507\",\"hostBSMId\": \"bsmXXXid\"}, \"trajectory\": { \"location\": {\"ecef_x\": 122, \"ecef_y\": 1,\"ecef_z\": 0,\"timestamp\": \"1632679657\"}}}";
    std::string mobilitypath_json_str_2 = "{\"metadata\": {\"timestamp\" : \"1632679658\",\"hostStaticId\": \"DOT-508\",\"hostBSMId\": \"bsmXXXid\"}, \"trajectory\": { \"location\": {\"ecef_x\": 122, \"ecef_y\": 1,\"ecef_z\": 0,\"timestamp\": \"1632679657\"}}}";
    std::string mobilitypath_json_str_3 = "{\"metadata\": {\"timestamp\" : \"1632679659\",\"hostStaticId\": \"DOT-509\",\"hostBSMId\": \"bsmXXXid\", \"targetStaticId\": \"NA\"}, \"trajectory\": { \"location\": {\"ecef_x\": 122, \"ecef_y\": 1,\"ecef_z\": 0,\"timestamp\": \"1632679657\"}, \"offsets\": {\"offset_x\": 122, \"offset_y\": 1,\"offset_z\": 0}}}";
    mobilitypath_w_obj.process_incoming_msg(mobilitypath_json_str_1);
    mobilitypath_w_obj.process_incoming_msg(mobilitypath_json_str_2);
    mobilitypath_w_obj.process_incoming_msg(mobilitypath_json_str_3);
    ASSERT_EQ(3, mobilitypath_w_obj.get_curr_list().size());
    mobilitypath_w_obj.pop_cur_element_from_list(0);
    ASSERT_EQ(2, mobilitypath_w_obj.get_curr_list().size());
}