#include <rapidjson/document.h>

#include "gtest/gtest.h"
#include "mobilityoperation_worker.h"

TEST(test_mobilityoperation_worker, get_curr_list)
{
    message_services::workers::mobilityoperation_worker mobilityoperation_w_obj;
    ASSERT_EQ(0, mobilityoperation_w_obj.get_curr_list().size());
}

TEST(test_mobilityoperation_worker, process_incoming_msg)
{
    message_services::workers::mobilityoperation_worker mobilityoperation_w_obj;
    ASSERT_EQ(0, mobilityoperation_w_obj.get_curr_list().size());
    std::string mobilityoperation_json_str = "{\"header\": {\"timestamp\" : 1632679657,\"sender_id\": \"DOT-507\",\"sender_bsm_id\": \"bsmXXXid1\"}}";
    mobilityoperation_w_obj.process_incoming_msg(mobilityoperation_json_str);
    ASSERT_EQ(1632679657, mobilityoperation_w_obj.get_curr_list().front().getHeader().timestamp);
    ASSERT_EQ("bsmXXXid1", mobilityoperation_w_obj.get_curr_list().front().getHeader().sender_bsm_id);
    ASSERT_EQ(1, mobilityoperation_w_obj.get_curr_list().size());
}

TEST(test_mobilityoperation_worker, pop_cur_element_from_list)
{
    message_services::workers::mobilityoperation_worker mobilityoperation_w_obj;
    ASSERT_EQ(0, mobilityoperation_w_obj.get_curr_list().size());
    std::string mobilityoperation_json_str_1 = "{\"header\": {\"timestamp\" : 1632679657,\"sender_id\": \"DOT-507\",\"sender_bsm_id\": \"bsmXXXid1\"}}";
    std::string mobilityoperation_json_str_2 = "{\"header\": {\"timestamp\" : 1632679658,\"sender_id\": \"DOT-508\",\"sender_bsm_id\": \"bsmXXXid2\"}, \"strategy\": \"NA\",\"strategy_params\": \"NA\"}";
    std::string mobilityoperation_json_str_3 = "{\"header\": {\"timestamp\" : 1632679659,\"sender_id\": \"DOT-509\",\"sender_bsm_id\": \"bsmXXXid3\"}, \"strategy\": \"NA\"}";
    mobilityoperation_w_obj.process_incoming_msg(mobilityoperation_json_str_1);
    mobilityoperation_w_obj.process_incoming_msg(mobilityoperation_json_str_2);
    mobilityoperation_w_obj.process_incoming_msg(mobilityoperation_json_str_3);
    ASSERT_EQ(3, mobilityoperation_w_obj.get_curr_list().size());
    mobilityoperation_w_obj.pop_cur_element_from_list(2);
    ASSERT_EQ(2, mobilityoperation_w_obj.get_curr_list().size());
}