
#include "gtest/gtest.h"
#include "vehicle.h"
#include "rapidjson/document.h"
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

#include "intersection_client.h"

TEST(test_vehicle, update)
{


        
    intersection_client localmap;        
    localmap.call();

    try{

        if (localmap.is_running() == true){
        
            vehicle test_veh;

            /* these set of tests make sure that the scheduling service has succesfully added the vehicle information for the first time */
            spdlog::info("* these set of tests make sure that the scheduling service has succesfully added the vehicle information for the first time *");
            spdlog::info("");

            const char* paylod = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-507\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": 5, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 5894, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 5894, \"link_lane_id\": 23016, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 5894, \"ds\": 7},{\"ts\": 1623677096200, \"id\": 5894, \"ds\": 6}, {\"ts\": 1623677096400, \"id\": 5894, \"ds\": 5}, {\"ts\": 1623677096600, \"id\": 5894, \"ds\": 4}]}}";

            rapidjson::Document message;
            message.SetObject();
            message.Parse(paylod);

            spdlog::info("adding the vehicle info for the first time. payload: {0}", paylod);
            test_veh.update(message, localmap);
            
            ASSERT_EQ("DOT-507", test_veh.get_id());
            spdlog::info("vehicle id: {0}", test_veh.get_id());
            ASSERT_EQ(5, test_veh.get_length());
            ASSERT_EQ(10, test_veh.get_minGap());
            ASSERT_EQ(1.5, test_veh.get_reactionT());
            ASSERT_EQ(5, test_veh.get_accelMax());
            ASSERT_EQ(-5, test_veh.get_decelMax());
            ASSERT_EQ("straight", test_veh.get_direction());
            ASSERT_EQ("5894", test_veh.get_entryLaneID());
            ASSERT_EQ("23016", test_veh.get_linkID());
            ASSERT_LE(0, test_veh.get_linkPriority());
            ASSERT_LE(0, test_veh.get_departurePosition());
            ASSERT_LE(0, test_veh.get_flexET());
            ASSERT_LE(0, test_veh.get_flexST());
            ASSERT_EQ(false, test_veh.get_access());

            ASSERT_EQ(1623677096.000, test_veh.get_curTime());
            spdlog::info("current timestamp: {0}", test_veh.get_curTime());
            ASSERT_EQ(7, test_veh.get_curDistance());
            spdlog::info("current distance from the end of the lane: {0}", test_veh.get_curDistance());
            /* the unit of the received speed from the message is mile per hour
            *  but the unit of the speed defined in the vehicle class is meter per second. 
            *  Therefore, a conversion has been added here.
            */
            ASSERT_EQ(5 * 0.44704, test_veh.get_curSpeed());
            spdlog::info("current speed: {0}", test_veh.get_curSpeed());
            ASSERT_EQ(0, test_veh.get_curAccel());
            ASSERT_EQ("5894", test_veh.get_curLaneID());
            spdlog::info("current lane id: {0}", test_veh.get_curLaneID());

            ASSERT_EQ("EV", test_veh.get_curState());
            spdlog::info("current state: {0}", test_veh.get_curState());
            EXPECT_EQ(-1, test_veh.get_actualST());
            EXPECT_EQ(-1, test_veh.get_actualET());
            EXPECT_EQ(-1, test_veh.get_actualDT());

            ASSERT_EQ(4, test_veh.get_futureInfo().size());
            for (int i = 0; i < (int)test_veh.get_futureInfo().size(); ++i){
                ASSERT_EQ(1623677096.000 + i*0.2, test_veh.get_futureInfo()[i].timestamp);
                ASSERT_EQ("5894", test_veh.get_futureInfo()[i].lane_id);
                ASSERT_EQ(7 - i*1, test_veh.get_futureInfo()[i].distance);
            }
            spdlog::info("");
            
            /* these set of tests make sure that the scheduling service has succesfully updated the vehicle information from the previous update */
            message["metadata"]["timestamp"].SetInt64(1623677097000);
            message["payload"]["cur_ds"].SetDouble(2);
            message["payload"]["cur_speed"].SetDouble(2);
            message["payload"]["cur_accel"].SetDouble(-3);
            spdlog::info("* updating the vehicle info with a new message *");
            spdlog::info("The changed information in the payload => timestamp: {0}, cur_ds: {1}, cur_speed: {2}", message["metadata"]["timestamp"].GetInt64(), message["payload"]["cur_ds"].GetDouble(), message["payload"]["cur_speed"].GetDouble());
            test_veh.update(message, localmap);
            ASSERT_EQ(1623677097.000, test_veh.get_curTime());
            spdlog::info("current timestamp: {0}", test_veh.get_curTime());
            ASSERT_EQ(2, test_veh.get_curDistance());
            spdlog::info("current distance from the end of the lane: {0}", test_veh.get_curDistance());
            /* the unit of the received speed from the message is mile per hour
            *  but the unit of the speed defined in the vehicle class is meter per second. 
            *  Therefore, a conversion has been added here.
            */
            ASSERT_EQ(2 * 0.44704, test_veh.get_curSpeed());
            spdlog::info("current speed: {0}", test_veh.get_curSpeed());
            ASSERT_EQ(-3, test_veh.get_curAccel());
            ASSERT_EQ("5894", test_veh.get_curLaneID());
            spdlog::info("current lane id: {0}", test_veh.get_curLaneID());
            ASSERT_EQ("EV", test_veh.get_curState());
            spdlog::info("current state: {0}", test_veh.get_curState());
            spdlog::info("");

            /* these set of tests make sure that the scheduling service does not replace the current vehicle information with an old update */
            message["metadata"]["timestamp"].SetInt64(1623677096000);
            message["payload"]["cur_ds"].SetDouble(7);
            message["payload"]["cur_speed"].SetDouble(5);
            message["payload"]["cur_accel"].SetDouble(0);
            spdlog::info("* make sure that the scheduling service does not replace the current vehicle information with an old update *");
            spdlog::info("The changed information in the payload => timestamp: {0}, cur_ds: {1}, cur_speed: {2}", message["metadata"]["timestamp"].GetInt64(), message["payload"]["cur_ds"].GetDouble(), message["payload"]["cur_speed"].GetDouble());
            test_veh.update(message, localmap);
            ASSERT_EQ(1623677097.000, test_veh.get_curTime());
            spdlog::info("current timestamp: {0}", test_veh.get_curTime());
            ASSERT_EQ(2, test_veh.get_curDistance());
            spdlog::info("current distance from the end of the lane: {0}", test_veh.get_curDistance());
            /* the unit of the received speed from the message is mile per hour
            *  but the unit of the speed defined in the vehicle class is meter per second. 
            *  Therefore, a conversion has been added here.
            */
            ASSERT_EQ(2 * 0.44704, test_veh.get_curSpeed());
            spdlog::info("current speed: {0}", test_veh.get_curSpeed());
            ASSERT_EQ(-3, test_veh.get_curAccel());
            ASSERT_EQ("5894", test_veh.get_curLaneID());
            spdlog::info("current lane id: {0}", test_veh.get_curLaneID());
            ASSERT_EQ("EV", test_veh.get_curState());
            spdlog::info("current state: {0}", test_veh.get_curState());
            spdlog::info("");

            /* these set of tests make sure that the scheduling service can succesfully updated the vehicle state */
            message["metadata"]["timestamp"].SetInt64(1623677098000);
            message["payload"]["cur_ds"].SetDouble(0);
            message["payload"]["cur_speed"].SetDouble(0);
            message["payload"]["cur_accel"].SetDouble(-2);
            spdlog::info("* make sure that the scheduling service can succesfully updated the vehicle state *");
            spdlog::info("The changed information in the payload => timestamp: {0}, cur_ds: {1}, cur_speed: {2}", message["metadata"]["timestamp"].GetInt64(), message["payload"]["cur_ds"].GetDouble(), message["payload"]["cur_speed"].GetDouble());
            test_veh.update(message, localmap);
            ASSERT_EQ(1623677098.000, test_veh.get_curTime());
            spdlog::info("current timestamp: {0}", test_veh.get_curTime());
            ASSERT_EQ(0, test_veh.get_curDistance());
            spdlog::info("current distance from the end of the lane: {0}", test_veh.get_curDistance());
            /* the unit of the received speed from the message is mile per hour
            *  but the unit of the speed defined in the vehicle class is meter per second. 
            *  Therefore, a conversion has been added here.
            */
            ASSERT_EQ(0 * 0.44704, test_veh.get_curSpeed());
            spdlog::info("current speed: {0}", test_veh.get_curSpeed());
            ASSERT_EQ(-2, test_veh.get_curAccel());
            ASSERT_EQ("5894", test_veh.get_curLaneID());
            spdlog::info("current lane id: {0}", test_veh.get_curLaneID());
            ASSERT_EQ("RDV", test_veh.get_curState());
            spdlog::info("current state: {0}", test_veh.get_curState());
            EXPECT_EQ(1623677098.000, test_veh.get_actualST());
            spdlog::info("actual stopping time: {0}", test_veh.get_actualST());
            spdlog::info("");
            
            message["metadata"]["timestamp"].SetInt64(1623677099000);
            message["payload"]["cur_accel"].SetDouble(0);
            spdlog::info("The changed information in the payload => timestamp: {0}, cur_ds: {1}, cur_speed: {2}", message["metadata"]["timestamp"].GetInt64(), message["payload"]["cur_ds"].GetDouble(), message["payload"]["cur_speed"].GetDouble());
            test_veh.update(message, localmap);
            ASSERT_EQ(1623677099.000, test_veh.get_curTime());
            spdlog::info("current timestamp: {0}", test_veh.get_curTime());
            ASSERT_EQ("RDV", test_veh.get_curState());
            spdlog::info("current state: {0}", test_veh.get_curState());
            EXPECT_EQ(1623677098.000, test_veh.get_actualST());
            spdlog::info("actual stopping time: {0}", test_veh.get_actualST());
            spdlog::info("");

            message["metadata"]["timestamp"].SetInt64(1623677100000);
            message["payload"]["is_allowed"].SetBool(true);
            spdlog::info("The changed information in the payload => timestamp: {0}, cur_ds: {1}, cur_speed: {2}, is_allowed: {3}", message["metadata"]["timestamp"].GetInt64(), message["payload"]["cur_ds"].GetDouble(), message["payload"]["cur_speed"].GetDouble(), message["payload"]["is_allowed"].GetBool());
            test_veh.update(message, localmap);
            ASSERT_EQ(1623677100.000, test_veh.get_curTime()); 
            spdlog::info("current timestamp: {0}", test_veh.get_curTime());
            ASSERT_EQ("23016", test_veh.get_curLaneID());
            spdlog::info("current lane id: {0}", test_veh.get_curLaneID());
            ASSERT_EQ(localmap.get_laneLength(test_veh.get_curLaneID()), test_veh.get_curDistance());
            spdlog::info("current distance from the end of the lane: {0}", test_veh.get_curDistance());
            ASSERT_EQ("DV", test_veh.get_curState());
            spdlog::info("current state: {0}", test_veh.get_curState());
            EXPECT_EQ(1623677098.000, test_veh.get_actualST());
            spdlog::info("actual stopping time: {0}", test_veh.get_actualST());
            EXPECT_EQ(1623677100.000, test_veh.get_actualET());
            spdlog::info("actual entering time: {0}", test_veh.get_actualET());
            spdlog::info("");

            message["metadata"]["timestamp"].SetInt64(1623677101000);
            message["payload"]["cur_lane_id"].SetInt(23016);
            message["payload"]["cur_ds"].SetDouble(18);
            message["payload"]["cur_speed"].SetDouble(2);
            message["payload"]["cur_accel"].SetDouble(2);
            spdlog::info("The changed information in the payload => timestamp: {0}, cur_ds: {1}, cur_speed: {2}, cur_lane_id: {3}", message["metadata"]["timestamp"].GetInt64(), message["payload"]["cur_ds"].GetDouble(), message["payload"]["cur_speed"].GetDouble(), message["payload"]["cur_lane_id"].GetInt());
            test_veh.update(message, localmap);
            ASSERT_EQ(1623677101.000, test_veh.get_curTime());
            spdlog::info("current timestamp: {0}", test_veh.get_curTime());
            ASSERT_EQ(18, test_veh.get_curDistance());
            spdlog::info("current distance from the end of the lane: {0}", test_veh.get_curDistance());
            /* the unit of the received speed from the message is mile per hour
            *  but the unit of the speed defined in the vehicle class is meter per second. 
            *  Therefore, a conversion has been added here.
            */
            ASSERT_EQ(2 * 0.44704, test_veh.get_curSpeed());
            spdlog::info("current speed: {0}", test_veh.get_curSpeed());
            ASSERT_EQ(2, test_veh.get_curAccel());
            ASSERT_EQ("23016", test_veh.get_curLaneID());
            spdlog::info("current lane id: {0}", test_veh.get_curLaneID());
            ASSERT_EQ("DV", test_veh.get_curState());
            spdlog::info("current state: {0}", test_veh.get_curState());
            EXPECT_EQ(1623677098.000, test_veh.get_actualST());
            spdlog::info("actual stopping time: {0}", test_veh.get_actualST());
            EXPECT_EQ(1623677100.000, test_veh.get_actualET());
            spdlog::info("actual entering time: {0}", test_veh.get_actualET());
            spdlog::info("");

            message["metadata"]["timestamp"].SetInt64(1623677105000);
            message["payload"]["cur_lane_id"].SetInt(11899);
            message["payload"]["cur_ds"].SetDouble(295);
            message["payload"]["cur_speed"].SetDouble(10);
            message["payload"]["cur_accel"].SetDouble(0);
            spdlog::info("The changed information in the payload => timestamp: {0}, cur_ds: {1}, cur_speed: {2}, cur_lane_id: {3}", message["metadata"]["timestamp"].GetInt64(), message["payload"]["cur_ds"].GetDouble(), message["payload"]["cur_speed"].GetDouble(), message["payload"]["cur_lane_id"].GetInt());
            test_veh.update(message, localmap);
            ASSERT_EQ(1623677105.000, test_veh.get_curTime());
            spdlog::info("current timestamp: {0}", test_veh.get_curTime());
            ASSERT_EQ(295, test_veh.get_curDistance());
            spdlog::info("current distance from the end of the lane: {0}", test_veh.get_curDistance());
            /* the unit of the received speed from the message is mile per hour
            *  but the unit of the speed defined in the vehicle class is meter per second. 
            *  Therefore, a conversion has been added here.
            */
            ASSERT_EQ(10 * 0.44704, test_veh.get_curSpeed());
            spdlog::info("current speed: {0}", test_veh.get_curSpeed());
            ASSERT_EQ(0, test_veh.get_curAccel());
            ASSERT_EQ("11899", test_veh.get_curLaneID());
            spdlog::info("current lane id: {0}", test_veh.get_curLaneID());
            ASSERT_EQ("LV", test_veh.get_curState());
            spdlog::info("current state: {0}", test_veh.get_curState());
            EXPECT_EQ(1623677098.000, test_veh.get_actualST());
            spdlog::info("actual stopping time: {0}", test_veh.get_actualST());
            EXPECT_EQ(1623677100.000, test_veh.get_actualET());
            spdlog::info("actual entering time: {0}", test_veh.get_actualET());
            EXPECT_EQ(1623677105.000, test_veh.get_actualDT());
            spdlog::info("actual departure time: {0}", test_veh.get_actualDT());
            spdlog::info("");
        }
        else{
            throw("the intersection model is not running");
        }
    }
    catch(...){
        spdlog::critical("the intersection model is not running");
    }

}


TEST(test_vehicle, set_departurePosition){

    intersection_client localmap;
    localmap.call();
    try{

        if (localmap.is_running() == true){

            vehicle test_veh;

            /* these set of tests make sure that the scheduling service has succesfully added the vehicle information for the first time */
            const char* paylod = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-507\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": 5, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 5894, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 5894, \"link_lane_id\": 23016, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 5894, \"ds\": 7},{\"ts\": 1623677096200, \"id\": 5894, \"ds\": 6}, {\"ts\": 1623677096400, \"id\": 5894, \"ds\": 5}, {\"ts\": 1623677096600, \"id\": 5894, \"ds\": 4}]}}";

            rapidjson::Document message;
            message.SetObject();
            message.Parse(paylod);

            test_veh.update(message, localmap);

            ASSERT_LE(0, test_veh.get_departurePosition());
            test_veh.set_departurePosition(5);
            ASSERT_EQ(5, test_veh.get_departurePosition());
        }
        else{
            throw("the intersection model is not running");
        }
    }
    catch(...){
        spdlog::critical("the intersection model is not running");
    }
}


TEST(test_vehicle, set_flexEt){

  
    intersection_client localmap;
    localmap.call();

    try{

        if (localmap.is_running() == true){

            vehicle test_veh;

            /* these set of tests make sure that the scheduling service has succesfully added the vehicle information for the first time */
            const char* paylod = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-507\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": 5, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 5894, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 5894, \"link_lane_id\": 23016, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 5894, \"ds\": 7},{\"ts\": 1623677096200, \"id\": 5894, \"ds\": 6}, {\"ts\": 1623677096400, \"id\": 5894, \"ds\": 5}, {\"ts\": 1623677096600, \"id\": 5894, \"ds\": 4}]}}";

            rapidjson::Document message;
            message.SetObject();
            message.Parse(paylod);

            test_veh.update(message, localmap);

            ASSERT_LE(0, test_veh.get_flexET());
            test_veh.set_flexEt(3);
            ASSERT_EQ(3, test_veh.get_flexET());
        }
        else{
            throw("the intersection model is not running");
        }
    }
    catch(...){
        spdlog::critical("the intersection model is not running");
    }
}


TEST(test_vehicle, set_flexST){

    intersection_client localmap;
    localmap.call();

    try{

        if (localmap.is_running() == true){
        
            vehicle test_veh;

            /* these set of tests make sure that the scheduling service has succesfully added the vehicle information for the first time */
            const char* paylod = "{\"metadata\": {\"timestamp\": 1623677096000}, \"payload\": {\"v_id\": \"DOT-507\", \"v_length\": 500, \"min_gap\": 10, \"react_t\": 1.5, \"max_accel\": 5, \"max_decel\": 5, \"cur_speed\": 5.0, \"cur_accel\": 0.0, \"cur_lane_id\": 5894, \"cur_ds\": 7.0, \"direction\": \"straight\", \"entry_lane_id\": 5894, \"link_lane_id\": 23016, \"is_allowed\": false, \"est_paths\": [{\"ts\": 1623677096000, \"id\": 5894, \"ds\": 7},{\"ts\": 1623677096200, \"id\": 5894, \"ds\": 6}, {\"ts\": 1623677096400, \"id\": 5894, \"ds\": 5}, {\"ts\": 1623677096600, \"id\": 5894, \"ds\": 4}]}}";

            rapidjson::Document message;
            message.SetObject();
            message.Parse(paylod);

            test_veh.update(message, localmap);

            ASSERT_LE(0, test_veh.get_flexST());
            test_veh.set_flexSt(4.6);
            ASSERT_EQ(4.6, test_veh.get_flexST());

        }
        else{
            throw("the intersection model is not running");
        }
    }
    catch(...){
        spdlog::critical("the intersection model is not running");
    }
}
