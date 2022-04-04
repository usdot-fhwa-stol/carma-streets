
#include <gtest/gtest.h>
#include <vector>
#include <set>
#include <spdlog/spdlog.h>

#include "intersection_client.h"
#include "vehicle.h"
#include "scheduling.h"


TEST(test_scheduling, scheduling)
{

    /* this unit test is conducted for the west intersection at TFHRC */
    intersection_client localmap;   
    localmap.call();
    unordered_map<string, vehicle> list_veh;
    set<string> list_veh_confirmation;
    set<string> list_veh_removal;

    try{

        if (localmap.is_running() == true){
            
            /* these set of test are conducted to make sure the scheduling class can successfuly store vehicle information */

            spdlog::info("* first, create a set of vehicles and update their information in the vehicle class *");
            spdlog::info(" adding 7 vehicles: 3 EVs, 2 RDVs, and 2 DVs.");

            string veh_id_1 = "DOT-501";
            list_veh[veh_id_1] = vehicle();
            const char* paylod_1 = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-501\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": -5, \"cur_speed\": 500.0, \"cur_accel\": 0.0, \"cur_lane_id\": 5894, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 5894, \"dest_lane_id\": 12459, \"link_lane_id\": 23016, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 5894, \"ds\": 7},{\"ts\": 1623677096200, \"id\": 5894, \"ds\": 6}, {\"ts\": 1623677096400, \"id\": 5894, \"ds\": 5}, {\"ts\": 1623677096600, \"id\": 5894, \"ds\": 4}]}}";
            rapidjson::Document message;
            message.SetObject();
            message.Parse(paylod_1);
            list_veh[veh_id_1].update(message, localmap);
            message["metadata"]["timestamp"].SetInt64(1623677097000);
            message["payload"]["cur_ds"].SetDouble(2);
            message["payload"]["cur_speed"].SetDouble(200);
            message["payload"]["cur_accel"].SetDouble(-3);
            message["payload"]["est_paths"][0]["ts"].SetInt64(1623677097000);
            message["payload"]["est_paths"][0]["ds"].SetDouble(2);
            message["payload"]["est_paths"][1]["ts"].SetInt64(1623677097200);
            message["payload"]["est_paths"][1]["ds"].SetDouble(1.6);
            message["payload"]["est_paths"][2]["ts"].SetInt64(1623677097400);
            message["payload"]["est_paths"][2]["ds"].SetDouble(1.2);
            message["payload"]["est_paths"][3]["ts"].SetInt64(1623677097600);
            message["payload"]["est_paths"][3]["ds"].SetDouble(0.8);
            list_veh[veh_id_1].update(message, localmap);


            string veh_id_2 = "DOT-502";
            list_veh[veh_id_2] = vehicle();
            const char* paylod_2 = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-502\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": -5, \"cur_speed\": 500.0, \"cur_accel\": 0.0, \"cur_lane_id\": 5894, \"cur_ds\": 30.0, \"direction\": \"straight\", \"entry_lane_id\": 5894, \"dest_lane_id\": 12459, \"link_lane_id\": 23016, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 5894, \"ds\": 30},{\"ts\": 1623677096200, \"id\": 5894, \"ds\": 29}, {\"ts\": 1623677096400, \"id\": 5894, \"ds\": 28}, {\"ts\": 1623677096600, \"id\": 5894, \"ds\": 27}]}}";
            message.Parse(paylod_2);
            list_veh[veh_id_2].update(message, localmap);
            message["metadata"]["timestamp"].SetInt64(1623677097000);
            message["payload"]["cur_ds"].SetDouble(25);
            message["payload"]["est_paths"][0]["ts"].SetInt64(1623677097000);
            message["payload"]["est_paths"][0]["ds"].SetDouble(25);
            message["payload"]["est_paths"][1]["ts"].SetInt64(1623677097200);
            message["payload"]["est_paths"][1]["ds"].SetDouble(24);
            message["payload"]["est_paths"][2]["ts"].SetInt64(1623677097400);
            message["payload"]["est_paths"][2]["ds"].SetDouble(23);
            message["payload"]["est_paths"][3]["ts"].SetInt64(1623677097600);
            message["payload"]["est_paths"][3]["ds"].SetDouble(22);
            list_veh[veh_id_2].update(message, localmap);

            string veh_id_3 = "DOT-503";
            list_veh[veh_id_3] = vehicle();
            const char* paylod_3 = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-503\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": -5, \"cur_speed\": 500.0, \"cur_accel\": 0.0, \"cur_lane_id\": 19252, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 19252, \"dest_lane_id\": 12459, \"link_lane_id\": 22878, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 19252, \"ds\": 7},{\"ts\": 1623677096200, \"id\": 19252, \"ds\": 6}, {\"ts\": 1623677096400, \"id\": 19252, \"ds\": 5}, {\"ts\": 1623677096600, \"id\": 19252, \"ds\": 4}]}}";
            message.Parse(paylod_3);
            list_veh[veh_id_3].update(message, localmap);
            message["metadata"]["timestamp"].SetInt64(1623677097000);
            message["payload"]["cur_ds"].SetDouble(2);
            message["payload"]["cur_speed"].SetDouble(200);
            message["payload"]["cur_accel"].SetDouble(-3);
            message["payload"]["est_paths"][0]["ts"].SetInt64(1623677097000);
            message["payload"]["est_paths"][0]["ds"].SetDouble(2);
            message["payload"]["est_paths"][1]["ts"].SetInt64(1623677097200);
            message["payload"]["est_paths"][1]["ds"].SetDouble(1.6);
            message["payload"]["est_paths"][2]["ts"].SetInt64(1623677097400);
            message["payload"]["est_paths"][2]["ds"].SetDouble(1.2);
            message["payload"]["est_paths"][3]["ts"].SetInt64(1623677097600);
            message["payload"]["est_paths"][3]["ds"].SetDouble(0.8);
            list_veh[veh_id_3].update(message, localmap);

            string veh_id_4 = "DOT-504";
            list_veh[veh_id_4] = vehicle();
            const char* paylod_4 = "{\"metadata\": {\"timestamp\": 1623677095000}, \"payload\": {\"v_id\": \"DOT-504\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": -5, \"cur_speed\": 100.0, \"cur_accel\": 0.0, \"cur_lane_id\": 13021, \"cur_ds\": 2.0, \"direction\": \"straight\", \"entry_lane_id\": 13021, \"dest_lane_id\": 12459, \"link_lane_id\": 22414, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677095000, \"id\": 13021, \"ds\": 2},{\"ts\": 1623677095200, \"id\": 13021, \"ds\": 1.6}, {\"ts\": 1623677095400, \"id\": 13021, \"ds\": 1.2}, {\"ts\": 1623677095600, \"id\": 13021, \"ds\": 0.8}]}}";
            message.Parse(paylod_4);
            list_veh[veh_id_4].update(message, localmap);
            message["metadata"]["timestamp"].SetInt64(1623677096000);
            message["payload"]["cur_ds"].SetDouble(0);
            message["payload"]["cur_speed"].SetDouble(0);
            message["payload"]["cur_accel"].SetDouble(-2);
            message["payload"]["est_paths"][0]["ts"].SetInt64(1623677096000);
            message["payload"]["est_paths"][0]["ds"].SetDouble(0);
            message["payload"]["est_paths"][1]["ts"].SetInt64(1623677096200);
            message["payload"]["est_paths"][1]["ds"].SetDouble(0);
            message["payload"]["est_paths"][2]["ts"].SetInt64(1623677096400);
            message["payload"]["est_paths"][2]["ds"].SetDouble(0);
            message["payload"]["est_paths"][3]["ts"].SetInt64(1623677096600);
            message["payload"]["est_paths"][3]["ds"].SetDouble(0);
            list_veh[veh_id_4].update(message, localmap);
            message["metadata"]["timestamp"].SetInt64(1623677097000);
            message["payload"]["cur_accel"].SetDouble(0);
            message["payload"]["est_paths"][0]["ts"].SetInt64(1623677097000);
            message["payload"]["est_paths"][1]["ts"].SetInt64(1623677097200);
            message["payload"]["est_paths"][2]["ts"].SetInt64(1623677097400);
            message["payload"]["est_paths"][3]["ts"].SetInt64(1623677097600);
            list_veh[veh_id_4].update(message, localmap);

            string veh_id_5 = "DOT-505";
            list_veh[veh_id_5] = vehicle();
            const char* paylod_5 = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-505\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": -5, \"cur_speed\": 100.0, \"cur_accel\": 0.0, \"cur_lane_id\": 16392, \"cur_ds\": 2.0, \"direction\": \"straight\", \"entry_lane_id\": 16392, \"dest_lane_id\": 12459, \"link_lane_id\": 22284, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 16392, \"ds\": 2},{\"ts\": 1623677096200, \"id\": 16392, \"ds\": 1.6}, {\"ts\": 1623677096400, \"id\": 16392, \"ds\": 1.2}, {\"ts\": 1623677096600, \"id\": 16392, \"ds\": 0.8}]}}";
            message.Parse(paylod_5);
            list_veh[veh_id_5].update(message, localmap);
            message["metadata"]["timestamp"].SetInt64(1623677097000);
            message["payload"]["cur_ds"].SetDouble(0);
            message["payload"]["cur_speed"].SetDouble(0);
            message["payload"]["cur_accel"].SetDouble(-2);
            message["payload"]["est_paths"][0]["ts"].SetInt64(1623677097000);
            message["payload"]["est_paths"][0]["ds"].SetDouble(0);
            message["payload"]["est_paths"][1]["ts"].SetInt64(1623677097200);
            message["payload"]["est_paths"][1]["ds"].SetDouble(0);
            message["payload"]["est_paths"][2]["ts"].SetInt64(1623677097400);
            message["payload"]["est_paths"][2]["ds"].SetDouble(0);
            message["payload"]["est_paths"][3]["ts"].SetInt64(1623677097600);
            message["payload"]["est_paths"][3]["ds"].SetDouble(0);
            list_veh[veh_id_5].update(message, localmap);

            string veh_id_6 = "DOT-506";
            list_veh[veh_id_6] = vehicle();
            const char* paylod_6 = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-506\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": -5, \"cur_speed\": 500.0, \"cur_accel\": 0.0, \"cur_lane_id\": 23578, \"cur_ds\": 18.0, \"direction\": \"straight\", \"entry_lane_id\": 19252, \"dest_lane_id\": 12459, \"link_lane_id\": 23578, \"is_allowed\": true, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 23578, \"ds\": 18},{\"ts\": 1623677096200, \"id\": 23578, \"ds\": 17}, {\"ts\": 1623677096400, \"id\": 23578, \"ds\": 16}, {\"ts\": 1623677096600, \"id\": 23578, \"ds\": 15}]}}";
            message.Parse(paylod_6);
            list_veh[veh_id_6].update(message, localmap);
            message["metadata"]["timestamp"].SetInt64(1623677097000);
            message["payload"]["cur_ds"].SetDouble(13);
            message["payload"]["est_paths"][0]["ts"].SetInt64(1623677097000);
            message["payload"]["est_paths"][0]["ds"].SetDouble(13);
            message["payload"]["est_paths"][1]["ts"].SetInt64(1623677097200);
            message["payload"]["est_paths"][1]["ds"].SetDouble(12);
            message["payload"]["est_paths"][2]["ts"].SetInt64(1623677097400);
            message["payload"]["est_paths"][2]["ds"].SetDouble(11);
            message["payload"]["est_paths"][3]["ts"].SetInt64(1623677097600);
            message["payload"]["est_paths"][3]["ds"].SetDouble(10);
            list_veh[veh_id_6].update(message, localmap);
            list_veh_confirmation.insert(veh_id_6);

            string veh_id_7 = "DOT-507";
            list_veh[veh_id_7] = vehicle();
            const char* paylod_7 = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-507\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": -5, \"cur_speed\": 500.0, \"cur_accel\": 0.0, \"cur_lane_id\": 22284, \"cur_ds\": 18.0, \"direction\": \"straight\", \"entry_lane_id\": 5894, \"dest_lane_id\": 12459, \"link_lane_id\": 22284, \"is_allowed\": true, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 22284, \"ds\": 18},{\"ts\": 1623677096200, \"id\": 22284, \"ds\": 17}, {\"ts\": 1623677096400, \"id\": 22284, \"ds\": 16}, {\"ts\": 1623677096600, \"id\": 22284, \"ds\": 15}]}}";
            message.Parse(paylod_7);
            list_veh[veh_id_7].update(message, localmap);
            message["metadata"]["timestamp"].SetInt64(1623677097000);
            message["payload"]["cur_ds"].SetDouble(13);
            message["payload"]["est_paths"][0]["ts"].SetInt64(1623677097000);
            message["payload"]["est_paths"][0]["ds"].SetDouble(13);
            message["payload"]["est_paths"][1]["ts"].SetInt64(1623677097200);
            message["payload"]["est_paths"][1]["ds"].SetDouble(12);
            message["payload"]["est_paths"][2]["ts"].SetInt64(1623677097400);
            message["payload"]["est_paths"][2]["ds"].SetDouble(11);
            message["payload"]["est_paths"][3]["ts"].SetInt64(1623677097600);
            message["payload"]["est_paths"][3]["ds"].SetDouble(10);
            list_veh[veh_id_7].update(message, localmap);
            list_veh_confirmation.insert(veh_id_7);


            scheduling schedule_1(list_veh, list_veh_confirmation, localmap, list_veh_removal);
            schedule_1.set_timestamp(1623677097.000);
            ASSERT_EQ(7, schedule_1.get_vehicleIdList().size());
            ASSERT_EQ(7, schedule_1.get_vehicleIndexList().size());
            ASSERT_EQ(7, schedule_1.get_timeList().size());
            ASSERT_EQ(7, schedule_1.get_speedList().size());
            ASSERT_EQ(7, schedule_1.get_accelList().size());
            ASSERT_EQ(7, schedule_1.get_laneIdList().size());
            ASSERT_EQ(7, schedule_1.get_stateList().size());
            ASSERT_EQ(7, schedule_1.get_departPosIndexList().size());
            ASSERT_EQ(7, schedule_1.get_distanceList().size());
            ASSERT_EQ(7, schedule_1.get_clearTimeList().size());
            ASSERT_EQ(7, schedule_1.get_estList().size());
            ASSERT_EQ(7, schedule_1.get_stList().size());
            ASSERT_EQ(7, schedule_1.get_dtList().size());
            spdlog::info("total number of vehicles in the schedule class: {0}", schedule_1.get_vehicleIdList().size());

            ASSERT_EQ(4, schedule_1.get_indexEVs().size());
            spdlog::info("total number of entry lanes: {0}", schedule_1.get_indexEVs().size());

            ASSERT_EQ(2, schedule_1.get_indexEVs()[0].size());
            spdlog::info("number of EVs in entry lane {0}: {1}", localmap.get_laneIdEntry()[0], schedule_1.get_indexEVs()[0].size());

            ASSERT_EQ(1, schedule_1.get_indexEVs()[1].size());
            spdlog::info("number of EVs in entry lane {0}: {1}", localmap.get_laneIdEntry()[1], schedule_1.get_indexEVs()[1].size());

            ASSERT_EQ(0, schedule_1.get_indexEVs()[2].size());
            spdlog::info("number of EVs in entry lane {0}: {1}", localmap.get_laneIdEntry()[2], schedule_1.get_indexEVs()[2].size());

            ASSERT_EQ(0, schedule_1.get_indexEVs()[3].size());
            spdlog::info("number of EVs in entry lane {0}: {1}", localmap.get_laneIdEntry()[3], schedule_1.get_indexEVs()[3].size());

            ASSERT_LE(schedule_1.get_distanceList()[schedule_1.get_indexEVs()[0][0]], schedule_1.get_distanceList()[schedule_1.get_indexEVs()[0][1]]);
            spdlog::info("test whether EVs in each entry lane are sorted based on their distance from the end of lane");

            ASSERT_EQ(2, schedule_1.get_indexRDVs().size());
            spdlog::info("number of RDVs: {0}", schedule_1.get_indexRDVs().size());

            ASSERT_LE(schedule_1.get_departPosIndexList()[schedule_1.get_indexRDVs()[0]], schedule_1.get_departPosIndexList()[schedule_1.get_indexRDVs()[1]]);
            spdlog::info("test whether RDVs are sorted based on their departure position");

            ASSERT_EQ(2, schedule_1.get_indexDVs().size());
            spdlog::info("number of DVs: {0}", schedule_1.get_indexDVs().size());

            ASSERT_LE(schedule_1.get_departPosIndexList()[schedule_1.get_indexDVs()[0]], schedule_1.get_departPosIndexList()[schedule_1.get_indexDVs()[1]]);
            spdlog::info("test whether DVs are sorted based on their departure position");

            spdlog::info("test whether the order of vehicle information in all scheduling lists are the same.");
            for (int i = 0; i < (int)schedule_1.get_vehicleIdList().size(); ++i){
                string vehicle_id = schedule_1.get_vehicleIdList()[i];
                ASSERT_EQ(schedule_1.get_laneIdList()[i], list_veh[vehicle_id].get_curLaneID());
            }

            spdlog::info("");
            spdlog::info("test the set functions");

            schedule_1.set_st(0, 1623677098.000);
            ASSERT_EQ(1623677098.000, schedule_1.get_stList()[0]);
            schedule_1.set_et(0, 1623677099.000);
            ASSERT_EQ(1623677099.000, schedule_1.get_etList()[0]);
            schedule_1.set_dt(0, 1623677100.000);
            ASSERT_EQ(1623677100.000, schedule_1.get_dtList()[0]);
            schedule_1.set_departPosIndex(0, 2);
            ASSERT_EQ(2, schedule_1.get_departPosIndexList()[0]);
            schedule_1.set_access(3, true);
            ASSERT_EQ(true, schedule_1.get_accessList()[3]);

        }
        else{
            throw("the intersection model is not running");
        }
    }
    catch(...){
        spdlog::critical("the intersection model is not running");
    }
}