#include <rapidjson/document.h>

#include "gtest/gtest.h"
#include "bsm_worker.h"

TEST(test_bsm_worker, get_curr_list)
{
    message_services::workers::bsm_worker bsm_w_obj;
    ASSERT_EQ(0, bsm_w_obj.get_curr_list().size());
}

TEST(test_bsm_worker, process_incoming_msg)
{
    message_services::workers::bsm_worker bsm_w_obj;
    ASSERT_EQ(0, bsm_w_obj.get_curr_list().size());
    std::string bsm_json_str = "{\"core_data\": {\"temprary_id\": \"bsmid1\",\"sec_mark\": \"1632369320\"}}";
    bsm_w_obj.process_incoming_msg(bsm_json_str);
    ASSERT_EQ(1632369320, bsm_w_obj.get_curr_list().front().getCore_data().sec_mark);
    ASSERT_EQ("bsmid1", bsm_w_obj.get_curr_list().front().getCore_data().temprary_id);
    ASSERT_EQ(1, bsm_w_obj.get_curr_list().size());
}

TEST(test_bsm_worker, pop_cur_element_from_list)
{
    message_services::workers::bsm_worker bsm_w_obj;
    ASSERT_EQ(0, bsm_w_obj.get_curr_list().size());
    std::string bsm_json_str_1 = "{\"core_data\": {\"temprary_id\": \"bsmid1\", \"latitude\":38.956287,\"longitude\" :-77.150492,\"sec_mark\": \"1632369320\",\"speed\": 15,\"size\": { \"length\": 8}}}";
    std::string bsm_json_str_2 = "{\"core_data\": {\"temprary_id\": \"bsmid2\", \"latitude\":38.956287,\"longitude\" :-77.150492,\"sec_mark\": \"1632369321\",\"speed\": 14,\"size\": { \"length\": 3}}}";
    std::string bsm_json_str_3 = "{\"core_data\": {\"temprary_id\": \"bsmid3\", \"latitude\":38.956287,\"longitude\" :-77.150492,\"sec_mark\": \"1632369322\", \"speed\": 13 ,\"size\": { \"length\": 12}}}";
    bsm_w_obj.process_incoming_msg(bsm_json_str_1);
    bsm_w_obj.process_incoming_msg(bsm_json_str_2);
    bsm_w_obj.process_incoming_msg(bsm_json_str_3);
    ASSERT_EQ(3, bsm_w_obj.get_curr_list().size());
    bsm_w_obj.pop_cur_element_from_list(0);
    ASSERT_EQ(2, bsm_w_obj.get_curr_list().size());
}