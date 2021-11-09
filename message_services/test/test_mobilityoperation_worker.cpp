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
    std::string mobilityoperation_json_str = "{\"metadata\": {\"timestamp\" : \"1632679657\",\"hostStaticId\": \"DOT-507\",\"hostBSMId\": \"bsmXXXid1\"}}";
    mobilityoperation_w_obj.process_incoming_msg(mobilityoperation_json_str);
    ASSERT_EQ(1632679657, mobilityoperation_w_obj.get_curr_list().front().getHeader().timestamp);
    ASSERT_EQ("bsmXXXid1", mobilityoperation_w_obj.get_curr_list().front().getHeader().sender_bsm_id);
    ASSERT_EQ(1, mobilityoperation_w_obj.get_curr_list().size());
}

TEST(test_mobilityoperation_worker, pop_cur_element_from_list)
{
    message_services::workers::mobilityoperation_worker mobilityoperation_w_obj;
    ASSERT_EQ(0, mobilityoperation_w_obj.get_curr_list().size());
    std::string mobilityoperation_json_str_1 = "{\"metadata\": {\"timestamp\" : \"1632679657\",\"hostStaticId\": \"DOT-507\",\"hostBSMId\": \"bsmXXXid1\"}}";
    std::string mobilityoperation_json_str_2 = "{\"metadata\": {\"timestamp\" : \"1632679658\",\"hostStaticId\": \"DOT-508\",\"hostBSMId\": \"bsmXXXid2\"}, \"strategy\": \"NA\",\"strategy_params\": \"msg_count: 12,access: 0, max_accel: 1.500000, max_decel:-1.000000,react_time: 4.500000, min_gap: 5.000000, depart_pos: 9999,turn_direction:straight\"}";
    std::string mobilityoperation_json_str_3 = "{\"metadata\": {\"timestamp\" : \"1632679659\",\"hostStaticId\": \"DOT-509\",\"hostBSMId\": \"bsmXXXid3\"}, \"strategy\": \"NA\"}";
    mobilityoperation_w_obj.process_incoming_msg(mobilityoperation_json_str_1);
    mobilityoperation_w_obj.process_incoming_msg(mobilityoperation_json_str_2);
    mobilityoperation_w_obj.process_incoming_msg(mobilityoperation_json_str_3);
    ASSERT_EQ(3, mobilityoperation_w_obj.get_curr_list().size());
    mobilityoperation_w_obj.pop_cur_element_from_list(0);
    ASSERT_EQ(2, mobilityoperation_w_obj.get_curr_list().size());
    ASSERT_EQ("12", mobilityoperation_w_obj.get_curr_list().front().get_value_from_strategy_params("msg_count"));
    ASSERT_EQ("0", mobilityoperation_w_obj.get_curr_list().front().get_value_from_strategy_params("access"));
    ASSERT_EQ("1.500000", mobilityoperation_w_obj.get_curr_list().front().get_value_from_strategy_params("max_accel"));
    ASSERT_EQ("-1.000000", mobilityoperation_w_obj.get_curr_list().front().get_value_from_strategy_params("max_decel"));
    ASSERT_EQ("4.500000", mobilityoperation_w_obj.get_curr_list().front().get_value_from_strategy_params("react_time"));
    ASSERT_EQ("5.000000", mobilityoperation_w_obj.get_curr_list().front().get_value_from_strategy_params("min_gap"));
    ASSERT_EQ("9999", mobilityoperation_w_obj.get_curr_list().front().get_value_from_strategy_params("depart_pos"));
    ASSERT_EQ("straight", mobilityoperation_w_obj.get_curr_list().front().get_value_from_strategy_params("turn_direction"));
}