#include <rapidjson/document.h>

#include "gtest/gtest.h"
#include "vehicle_status_intent_worker.h"

TEST(test_vehicle_status_intent_worker, get_curr_map)
{
    message_services::workers::vehicle_status_intent_worker vsi_w_obj;
    ASSERT_EQ(0, vsi_w_obj.get_curr_map().size());
}

TEST(test_vehicle_status_intent_worker, update_by_incoming_bsm_msg)
{
    message_services::workers::vehicle_status_intent_worker vsi_w_obj;
    message_services::models::bsm bsm_obj;
    vsi_w_obj.update_by_incoming_bsm_msg(bsm_obj);
    ASSERT_EQ(0, vsi_w_obj.get_curr_map().size());
}

TEST(test_vehicle_status_intent_worker, update_insert_by_incoming_mobilityoperation_msg)
{
    message_services::workers::vehicle_status_intent_worker vsi_w_obj;
    message_services::models::bsm bsm_obj;
    message_services::models::mobilityoperation mo_obj;
    message_services::models::mobility_header_t mo_header;
    mo_header.sender_bsm_id = "bsmXXXId";
    mo_header.sender_id = "DOT-507";
    mo_header.timestamp = 3239293903;
    mo_obj.setHeader(mo_header);
    vsi_w_obj.update_insert_by_incoming_mobilityoperation_msg(mo_obj);
    ASSERT_EQ(1, vsi_w_obj.get_curr_map().size());

    message_services::models::bsmCoreData_t core_data;
    core_data.temprary_id = "bsmXXXId_2";
    core_data.speed = 15;
    bsm_obj.setCore_data(core_data);
    vsi_w_obj.update_by_incoming_bsm_msg(bsm_obj);
    ASSERT_EQ(1, vsi_w_obj.get_curr_map().size());
    std::cout << vsi_w_obj.get_curr_map().find("DOT-507")->first << vsi_w_obj.get_curr_map().find("DOT-507")->second << std::endl;
}

TEST(test_vehicle_status_intent_worker, update_insert_by_incoming_mobilitypath_msg)
{
    message_services::workers::vehicle_status_intent_worker vsi_w_obj;
    message_services::models::bsm bsm_obj;
    message_services::models::mobilityoperation mp_obj;
    message_services::models::mobility_header_t mp_header;
    message_services::models::mobilityoperation mo_obj;
    message_services::models::mobility_header_t mo_header;
    message_services::models::bsmCoreData_t core_data;

    mo_header.sender_bsm_id = "bsmXXXId_2";
    mo_header.sender_id = "DOT-507";
    mo_header.timestamp = 3239293903;
    mo_obj.setHeader(mo_header);
    vsi_w_obj.update_insert_by_incoming_mobilityoperation_msg(mo_obj);

    core_data.temprary_id = "bsmXXXId_2";
    core_data.speed = 15;
    bsm_obj.setCore_data(core_data);
    vsi_w_obj.update_by_incoming_bsm_msg(bsm_obj);
    ASSERT_EQ(1, vsi_w_obj.get_curr_map().size());

    mp_header.sender_bsm_id = "bsmXXXId_3";
    mp_header.sender_id = "DOT-507";
    mp_header.timestamp = 3239293904;
    mp_obj.setHeader(mp_header);
    vsi_w_obj.update_insert_by_incoming_mobilityoperation_msg(mp_obj);
    ASSERT_EQ(1, vsi_w_obj.get_curr_map().size());

    core_data.temprary_id = "bsmXXXId_3";
    core_data.speed = 20;
    bsm_obj.setCore_data(core_data);
    vsi_w_obj.update_by_incoming_bsm_msg(bsm_obj);
    ASSERT_EQ(1, vsi_w_obj.get_curr_map().size());

}

TEST(test_vehicle_status_intent_worker, pop_element_from_map)
{
    message_services::workers::vehicle_status_intent_worker vsi_w_obj;
    ASSERT_EQ(0, vsi_w_obj.get_curr_map().size());

    message_services::models::mobilitypath mp_obj;
    message_services::models::mobility_header_t mp_header;
    mp_header.sender_bsm_id = "bsmXXXId";
    mp_header.sender_id = "DOT-507";
    mp_header.timestamp = 3239293902;
    mp_obj.setHeader(mp_header);
    vsi_w_obj.update_insert_by_incoming_mobilitypath_msg(mp_obj);
    ASSERT_EQ(1, vsi_w_obj.get_curr_map().size());

    vsi_w_obj.pop_element_from_map("DOT-507");
    ASSERT_EQ(0, vsi_w_obj.get_curr_map().size());
}