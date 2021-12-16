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
    std::string bsm_json_str = "{\"core_data\": {\"id\": \"bsmid1\",\"sec_mark\": \"1632369320\"}}";
    bsm_w_obj.process_incoming_msg(bsm_json_str);
    ASSERT_EQ(1632369320, bsm_w_obj.get_curr_map().begin()->second.getCore_data().sec_mark);
    ASSERT_EQ("bsmid1", bsm_w_obj.get_curr_map().begin()->second.getCore_data().temprary_id);
    ASSERT_EQ(1, bsm_w_obj.get_curr_map().size());
}

TEST(test_bsm_worker, pop_cur_element_from_list)
{
    message_services::workers::bsm_worker bsm_w_obj;
    ASSERT_EQ(0, bsm_w_obj.get_curr_map().size());
    std::string bsm_json_str_1 = "{\"core_data\": {\"id\": \"bsmid1\", \"lat\":\"38.956287\",\"long\" :\"-77.150492\",\"sec_mark\": \"16323\",\"speed\": \"15\",\"size\": { \"length\": \"8\"}}}";
    std::string bsm_json_str_2 = "{\"core_data\": {\"id\": \"bsmid2\", \"lat\":\"38.956287\",\"long\" :\"-77.150492\",\"sec_mark\": \"16323\",\"speed\": \"14\",\"size\": { \"length\": \"3\"}}}";
    std::string bsm_json_str_3 = "{\"core_data\": {\"id\": \"bsmid3\", \"lat\":\"38.956287\",\"long\" :\"-77.150492\",\"elev\" :\"72\",\"sec_mark\": \"16323\",\"msg_count\": \"12\", \"speed\": \"13\" ,\"size\": { \"length\": \"12\"}}}";
    bsm_w_obj.process_incoming_msg(bsm_json_str_1);
    bsm_w_obj.process_incoming_msg(bsm_json_str_2);
    bsm_w_obj.process_incoming_msg(bsm_json_str_3);
    ASSERT_EQ(3, bsm_w_obj.get_curr_map().size());
    bsm_w_obj.pop_cur_element_from_list(0);
    ASSERT_EQ(2, bsm_w_obj.get_curr_map().size());
    ASSERT_EQ("bsmid3", bsm_w_obj.get_curr_map().begin()->second.getCore_data().temprary_id);
    ASSERT_EQ(38.956287, bsm_w_obj.get_curr_map().begin()->second.getCore_data().latitude);
    ASSERT_EQ(72, bsm_w_obj.get_curr_map().begin()->second.getCore_data().elev);
    ASSERT_EQ(-77.150492, bsm_w_obj.get_curr_map().begin()->second.getCore_data().longitude);
    ASSERT_EQ(13, bsm_w_obj.get_curr_map().begin()->second.getCore_data().speed);
    ASSERT_EQ(12, bsm_w_obj.get_curr_map().begin()->second.getCore_data().size.length);
    ASSERT_EQ(16323, bsm_w_obj.get_curr_map().begin()->second.getCore_data().sec_mark);
    ASSERT_EQ(12, bsm_w_obj.get_curr_map().begin()->second.getCore_data().msg_count);
}