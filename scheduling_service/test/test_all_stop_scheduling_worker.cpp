
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "vehicle_list.h"
#include "all_stop_vehicle_scheduler.h"
#include "all_stop_scheduling_worker.h"


using namespace streets_vehicles;
using namespace streets_vehicle_scheduler;
using namespace scheduling_service;

namespace {

    class all_stop_scheduling_worker_test : public ::testing::Test {

    protected:
        
        std::unique_ptr<all_stop_scheduling_worker> scheduling_worker;

        std::unordered_map<std::string, vehicle> veh_map;

        std::shared_ptr<all_stop_vehicle_scheduler> scheduler;

        
        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            
            scheduling_worker = std::unique_ptr<all_stop_scheduling_worker>(new all_stop_scheduling_worker());
            
            OpenAPI::OAIIntersection_info info;
            std::string json = "{\"departure_lanelets\":[{ \"id\":162, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":164, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":168, \"length\":34.130869420842046, \"speed_limit\":11.176 } ], \"entry_lanelets\":[ { \"id\":167, \"length\":195.73023157287864, \"speed_limit\":11.176 }, { \"id\":171, \"length\":34.130869411176431136, \"speed_limit\":11.176 }, { \"id\":163, \"length\":41.60952435603712, \"speed_limit\":11.176 } ], \"id\":9001, \"link_lanelets\":[ { \"conflict_lanelet_ids\":[ 161 ], \"id\":169, \"length\":15.85409574709938, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 165, 156, 161 ], \"id\":155, \"length\":16.796388658952235, \"speed_limit\":4.4704 }, { \"conflict_lanelet_ids\":[ 155, 161, 160 ], \"id\":165, \"length\":15.853947840111768943, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 155 ], \"id\":156, \"length\":9.744590320260139, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 169, 155, 165 ], \"id\":161, \"length\":16.043077028554038, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 165 ], \"id\":160, \"length\":10.295559117055083, \"speed_limit\":11.176 } ], \"name\":\"WestIntersection\"}";
            info.fromJson(QString::fromStdString(json));
            std::shared_ptr<OpenAPI::OAIIntersection_info> intersection = std::make_shared<OpenAPI::OAIIntersection_info>(info);
            SPDLOG_INFO("Intersection information consists of {0} entry lanes {1} linking lanes and {2} departure lanes.", 
                        intersection->getEntryLanelets().size(),
                        intersection->getLinkLanelets().size(), 
                        intersection->getDepartureLanelets().size()) ;


            scheduler = std::shared_ptr<all_stop_vehicle_scheduler>(new all_stop_vehicle_scheduler());
            scheduler->set_flexibility_limit(2);
            scheduler->set_intersection_info(intersection);

            SPDLOG_INFO("Test SetUp Complete");
        }
            
        /**
         * @brief Test TearDown method run after each test.
         * 
         */
        void TearDown() override{
            
        }

    };
};


TEST_F(all_stop_scheduling_worker_test, start_next_schedule_false)
{
    u_int64_t scheduling_delta = 2000;
    u_int64_t last_schedule_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    
    ASSERT_FALSE( scheduling_worker -> start_next_schedule(last_schedule_timestamp, scheduling_delta) );

}



TEST_F(all_stop_scheduling_worker_test, start_next_schedule_true)
{

    u_int64_t scheduling_delta = 2000;
    u_int64_t last_schedule_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    last_schedule_timestamp -= 2500;

    ASSERT_TRUE( scheduling_worker -> start_next_schedule(last_schedule_timestamp, scheduling_delta) );

}


TEST_F(all_stop_scheduling_worker_test, schedule_vehicles)
{
    u_int64_t current_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    vehicle veh_dv;
    veh_dv._id = "TEST_DV_01";
    veh_dv._accel_max = 2.0;
    veh_dv._decel_max = -2.0;
    veh_dv._cur_speed = 1.0;
    veh_dv._cur_accel = 2.0;
    veh_dv._cur_distance = 9.3;
    veh_dv._cur_lane_id = 160;
    veh_dv._cur_state = vehicle_state::DV;
    veh_dv._cur_time = current_timestamp;
    veh_dv._entry_lane_id = 163;
    veh_dv._link_id = 160;
    veh_dv._exit_lane_id = 164;
    veh_dv._direction = "right";
    veh_dv._departure_position = 1;
    veh_dv._access = true;
    veh_dv._actual_st = current_timestamp - 3000;
    veh_dv._actual_et = current_timestamp - 1000;

    vehicle veh_rdv1;
    veh_rdv1._id = "TEST_RDV_01";
    veh_rdv1._accel_max = 2.0;
    veh_rdv1._decel_max = -2.0;
    veh_rdv1._cur_speed = 0.0;
    veh_rdv1._cur_accel = 0.0;
    veh_rdv1._cur_distance = 1.0;
    veh_rdv1._cur_lane_id = 171;
    veh_rdv1._cur_state = vehicle_state::RDV;
    veh_rdv1._cur_time = current_timestamp;
    veh_rdv1._entry_lane_id = 171;
    veh_rdv1._link_id = 165;
    veh_rdv1._exit_lane_id = 164;
    veh_rdv1._direction = "straight";
    veh_rdv1._departure_position = 2;
    veh_rdv1._actual_st = current_timestamp - 1000;

    vehicle veh_rdv2;
    veh_rdv2._id = "TEST_RDV_02";
    veh_rdv2._accel_max = 2.0;
    veh_rdv2._decel_max = -2.0;
    veh_rdv2._cur_speed = 0.0;
    veh_rdv2._cur_accel = 0.0;
    veh_rdv2._cur_distance = 1.0;
    veh_rdv2._cur_lane_id = 167;
    veh_rdv2._cur_state = vehicle_state::RDV;
    veh_rdv2._cur_time = current_timestamp;
    veh_rdv2._entry_lane_id = 167;
    veh_rdv2._link_id = 169;
    veh_rdv2._exit_lane_id = 168;
    veh_rdv2._direction = "straight";
    veh_rdv2._departure_position = 3;
    veh_rdv2._actual_st = current_timestamp - 1000;

    veh_map.insert({{veh_dv._id, veh_dv}, {veh_rdv1._id, veh_rdv1}, {veh_rdv2._id, veh_rdv2}});


    intersection_schedule schedule = scheduling_worker -> schedule_vehicles(veh_map, scheduler);


    ASSERT_EQ( schedule.vehicle_schedules.size(), 3);
    vehicle_schedule veh_dv_schedule;
    vehicle_schedule veh_rdv1_schedule;
    vehicle_schedule veh_rdv2_schedule;
    for (auto veh_sched : schedule.vehicle_schedules){
        if (veh_sched.v_id == veh_dv._id){
            veh_dv_schedule = veh_sched;
        }
        else if (veh_sched.v_id == veh_rdv1._id){
            veh_rdv1_schedule = veh_sched;
        }
        else if (veh_sched.v_id == veh_rdv2._id){
            veh_rdv2_schedule = veh_sched;
        } 
    }

    ASSERT_EQ(veh_dv_schedule.v_id, veh_dv._id);
    ASSERT_EQ(veh_dv_schedule.est, veh_dv._actual_st);
    ASSERT_EQ(veh_dv_schedule.st, veh_dv._actual_st);
    ASSERT_EQ(veh_dv_schedule.et, veh_dv._actual_et);
    ASSERT_EQ(veh_dv_schedule.dp, 1);
    ASSERT_EQ(veh_dv_schedule.state, vehicle_state::DV);
    ASSERT_EQ(veh_dv_schedule.access, true);


    ASSERT_EQ(veh_rdv2_schedule.v_id, veh_rdv2._id);
    ASSERT_EQ(veh_rdv2_schedule.est, veh_rdv2._actual_st);
    ASSERT_EQ(veh_rdv2_schedule.st, veh_rdv2._actual_st);
    ASSERT_EQ(veh_rdv2_schedule.et, schedule.timestamp);
    ASSERT_EQ(veh_rdv2_schedule.dp, 2);
    ASSERT_EQ(veh_rdv2_schedule.state, vehicle_state::DV);
    ASSERT_EQ(veh_rdv2_schedule.access, true);


    ASSERT_EQ(veh_rdv1_schedule.v_id, veh_rdv1._id);
    ASSERT_EQ(veh_rdv1_schedule.est, veh_rdv1._actual_st);
    ASSERT_EQ(veh_rdv1_schedule.st, veh_rdv1._actual_st);
    ASSERT_EQ(veh_rdv1_schedule.et, std::max(veh_dv_schedule.dt, veh_rdv2_schedule.et));
    ASSERT_EQ(veh_rdv1_schedule.dp, 3);
    ASSERT_EQ(veh_rdv1_schedule.state, vehicle_state::RDV);
    ASSERT_EQ(veh_rdv1_schedule.access, false);

}


TEST_F(all_stop_scheduling_worker_test, convert_schedule_plan)
{

    intersection_schedule schedule;


    schedule.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


    vehicle_schedule veh_dv_schedule;
    veh_dv_schedule.v_id = "TEST_DV_01";
    veh_dv_schedule.est = schedule.timestamp - 3000;
    veh_dv_schedule.st = schedule.timestamp - 3000;
    veh_dv_schedule.et = schedule.timestamp - 1000;
    veh_dv_schedule.dt = schedule.timestamp + 3000;
    veh_dv_schedule.dp = 1;
    veh_dv_schedule.state = vehicle_state::DV;
    veh_dv_schedule.link_id = 160;
    veh_dv_schedule.entry_lane = 163;
    veh_dv_schedule.access = true;
    SPDLOG_DEBUG("Added schedule for {0} with dt {1}.", veh_dv_schedule.v_id, veh_dv_schedule.dt);
    // add vehicle_schedule
    schedule.vehicle_schedules.push_back(veh_dv_schedule);


    vehicle_schedule veh_rdv2_schedule;
    veh_rdv2_schedule.v_id = "TEST_RDV_02";
    veh_rdv2_schedule.est = schedule.timestamp - 1000;
    veh_rdv2_schedule.st = schedule.timestamp - 1000;
    veh_rdv2_schedule.et = schedule.timestamp;
    veh_rdv2_schedule.dt = schedule.timestamp + 4000;
    veh_rdv2_schedule.dp = 2;
    veh_rdv2_schedule.state = vehicle_state::DV;
    veh_rdv2_schedule.link_id = 169;
    veh_rdv2_schedule.entry_lane = 167;
    veh_rdv2_schedule.access = true;
    SPDLOG_DEBUG("Added schedule for {0} with dt {1}.", veh_rdv2_schedule.v_id, veh_rdv2_schedule.dt);
    // add vehicle_schedule
    schedule.vehicle_schedules.push_back(veh_rdv2_schedule);


    vehicle_schedule veh_rdv1_schedule;
    veh_rdv1_schedule.v_id = "TEST_RDV_01";
    veh_rdv1_schedule.est = schedule.timestamp - 1000;
    veh_rdv1_schedule.st = schedule.timestamp - 1000;
    veh_rdv1_schedule.et = std::max(veh_dv_schedule.dt, veh_rdv2_schedule.et);
    veh_rdv1_schedule.dt = veh_rdv1_schedule.et + 4000;
    veh_rdv1_schedule.dp = 3;
    veh_rdv1_schedule.state = vehicle_state::RDV;
    veh_rdv1_schedule.link_id = 165;
    veh_rdv1_schedule.entry_lane = 171;
    veh_rdv1_schedule.access = false;
    SPDLOG_DEBUG("Added schedule for {0} with dt {1}.", veh_rdv1_schedule.v_id, veh_rdv1_schedule.dt);
    // add vehicle_schedule
    schedule.vehicle_schedules.push_back(veh_rdv1_schedule);

    SPDLOG_INFO("size of schedule {0}: ", schedule.vehicle_schedules.size());

    std::string schedule_str = scheduling_worker -> create_schedule_plan(schedule);


    SPDLOG_INFO("schedule plan (string): {0}", schedule_str);

    size_t nPos;
    int count;

    nPos = schedule_str.find("metadata", 0);
    count = 0;
    while (nPos != std::string::npos){
        count++;
        nPos = schedule_str.find("metadata", nPos + 1);
    }
    SPDLOG_INFO("number of occurrences of metadata in the schedule plan : {0}", count);
    ASSERT_EQ(count, 1);


    nPos = schedule_str.find("timestamp", 0);
    count = 0;
    while (nPos != std::string::npos){
        count++;
        nPos = schedule_str.find("timestamp", nPos + 1);
    }
    SPDLOG_INFO("number of occurrences of timestamp in the schedule plan : {0}", count);
    ASSERT_EQ(count, 1);


    nPos = schedule_str.find("Carma/stop_controlled_intersection", 0);
    count = 0;
    while (nPos != std::string::npos){
        count++;
        nPos = schedule_str.find("Carma/stop_controlled_intersection", nPos + 1);
    }
    SPDLOG_INFO("number of occurrences of Carma/stop_controlled_intersection in the schedule plan : {0}", count);
    ASSERT_EQ(count, 1);


    nPos = schedule_str.find("payload", 0);
    count = 0;
    while (nPos != std::string::npos){
        count++;
        nPos = schedule_str.find("payload", nPos + 1);
    }
    SPDLOG_INFO("number of occurrences of payload in the schedule plan : {0}", count);
    ASSERT_EQ(count, 1);


    nPos = schedule_str.find("TEST_DV_01", 0);
    count = 0;
    while (nPos != std::string::npos){
        count++;
        nPos = schedule_str.find("TEST_DV_01", nPos + 1);
    }
    SPDLOG_INFO("number of occurrences of TEST_DV_01 in the schedule plan : {0}", count);
    ASSERT_EQ(count, 1);


    nPos = schedule_str.find("TEST_RDV_01", 0);
    count = 0;
    while (nPos != std::string::npos){
        count++;
        nPos = schedule_str.find("TEST_RDV_01", nPos + 1);
    }
    SPDLOG_INFO("number of occurrences of TEST_RDV_01 in the schedule plan : {0}", count);
    ASSERT_EQ(count, 1);


    nPos = schedule_str.find("TEST_RDV_02", 0);
    count = 0;
    while (nPos != std::string::npos){
        count++;
        nPos = schedule_str.find("TEST_RDV_02", nPos + 1);
    }
    SPDLOG_INFO("number of occurrences of TEST_RDV_02 in the schedule plan : {0}", count);
    ASSERT_EQ(count, 1);

}