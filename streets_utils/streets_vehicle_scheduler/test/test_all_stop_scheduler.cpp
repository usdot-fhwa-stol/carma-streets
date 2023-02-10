#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "vehicle_list.h"
#include "all_stop_vehicle_scheduler.h"
#include "all_stop_intersection_schedule.h"

using namespace streets_vehicles;
using namespace streets_vehicle_scheduler;
namespace {

    class all_stop_scheduler_test : public ::testing::Test {
    private:

       

    protected:
        std::unordered_map<std::string, vehicle> veh_list;

        std::unique_ptr<all_stop_vehicle_scheduler> scheduler;

        std::shared_ptr<intersection_schedule> schedule;

        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            scheduler = std::unique_ptr<all_stop_vehicle_scheduler>(new all_stop_vehicle_scheduler());
            scheduler->set_flexibility_limit(2);
            OpenAPI::OAIIntersection_info info;
            std::string json = "{\"departure_lanelets\":[{ \"id\":162, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":164, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":168, \"length\":34.130869420842046, \"speed_limit\":11.176 } ], \"entry_lanelets\":[ { \"id\":167, \"length\":195.73023157287864, \"speed_limit\":11.176 }, { \"id\":171, \"length\":34.130869411176431136, \"speed_limit\":11.176 }, { \"id\":163, \"length\":41.60952435603712, \"speed_limit\":11.176 } ], \"id\":9001, \"link_lanelets\":[ { \"conflict_lanelet_ids\":[ 161 ], \"id\":169, \"length\":15.85409574709938, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 165, 156, 161 ], \"id\":155, \"length\":16.796388658952235, \"speed_limit\":4.4704 }, { \"conflict_lanelet_ids\":[ 155, 161, 160 ], \"id\":165, \"length\":15.853947840111768943, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 155 ], \"id\":156, \"length\":9.744590320260139, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 169, 155, 165 ], \"id\":161, \"length\":16.043077028554038, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 165 ], \"id\":160, \"length\":10.295559117055083, \"speed_limit\":11.176 } ], \"name\":\"WestIntersection\"}";
            info.fromJson(QString::fromStdString(json));
            std::shared_ptr<OpenAPI::OAIIntersection_info> intersection = std::make_shared<OpenAPI::OAIIntersection_info>(info);
            scheduler->set_intersection_info(intersection);
            SPDLOG_INFO("Intersection information consists of {0} entry lanes {1} linking lanes and {2} departure lanes.", 
                        intersection->getEntryLanelets().size(),
                        intersection->getLinkLanelets().size(), 
                        intersection->getDepartureLanelets().size()) ;

        }
            
        /**
         * @brief Test TearDown method run after each test.
         * 
         */
        void TearDown() override{
            
        }
    };
};

TEST_F(all_stop_scheduler_test, schedule_empty_vehicle_list) {
    
    // intersection_schedule schedule;
    schedule = std::make_shared<all_stop_intersection_schedule>();
    schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<all_stop_intersection_schedule> (schedule);
    ASSERT_EQ( sched->vehicle_schedules.size(), 0);
    //  
}

/**
 * @brief Test case with single EV. Speed limit, acceleration/deceleration limits and intersection geometry should not allow this
 * EV to reach lanelet speed limit in entry lane or link lane. This means the vehicle never reaches a cruising speed.
 * 
 */
TEST_F(all_stop_scheduler_test, one_ev_without_cruising){
    // intersection_schedule schedule;
    schedule = std::make_shared<all_stop_intersection_schedule>();

    schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    vehicle veh;
    veh._id = "TEST01";
    veh._accel_max = 2.0;
    veh._decel_max = -1.5;
    veh._cur_speed = 4.4704;
    veh._cur_accel = 1.0;
    veh._cur_distance = 60;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 169;
    veh._exit_lane_id = 168;
    veh._direction = "straight";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list,schedule);
    auto sched = std::dynamic_pointer_cast<all_stop_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);
    double est_time =(sched->vehicle_schedules.front().est-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EST time for scheduler  : {0}  vs calculated {1} ", est_time, 10.080 );
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->timestamp+10080);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->vehicle_schedules.front().st);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->vehicle_schedules.front().et);
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 14.062);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp+14062);


}

/**
 * @brief Test case with two vehicles, one which has stopped updating and the other which is still updating. 
 * This test case is meant to confirm that when a single vehicle stops updating, other vehicles that are still 
 * updating will receive valid schedules that no longer consider the non-updating vehicle.
 * 
 */
TEST_F(all_stop_scheduler_test, two_vehicles_one_stops_updating){
    // intersection_schedule schedule;
    schedule = std::make_shared<all_stop_intersection_schedule>();

    schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // Update to date vehicle
    vehicle veh;
    veh._id = "TEST01";
    veh._accel_max = 2.0;
    veh._decel_max = -1.5;
    veh._cur_speed = 4.4704;
    veh._cur_accel = 1.0;
    veh._cur_distance = 60;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 169;
    veh._exit_lane_id = 168;
    veh._direction = "straight";
    veh_list.insert({veh._id,veh});

    vehicle veh2;
    veh2._id = "TEST02";
    veh2._accel_max = 2.0;
    veh2._decel_max = -1.5;
    veh2._cur_speed = 4.4704;
    veh2._cur_accel = 1.0;
    veh2._cur_distance = 30;
    veh2._cur_lane_id = 167;
    veh2._cur_state = vehicle_state::EV;
    veh2._cur_time = schedule->timestamp - 10000; // This vehicle update is 10 seconds old and should no longer be considered.
    veh2._entry_lane_id = 167;
    veh2._link_id = 169;
    veh2._exit_lane_id = 168;
    veh2._direction = "straight";
    veh_list.insert({veh2._id,veh2});

    scheduler->schedule_vehicles(veh_list,schedule);
    auto sched = std::dynamic_pointer_cast<all_stop_intersection_schedule> (schedule);

    // Schedule should only contain a single vehicle an be identical to if there was only one vehicle.
    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);
    double est_time =(sched->vehicle_schedules.front().est-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EST time for scheduler  : {0}  vs calculated {1} ", est_time, 10.080 );
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->timestamp+10080);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->vehicle_schedules.front().st);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->vehicle_schedules.front().et);
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 14.062);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp+14062);
    
}

/**
 * @brief Test case with two vehicles, one which vehicle has an invalid timestamp of a future time and the other with
 * a valid time stamp. This test case is meant to confirm that when a single vehicle has an invalid timestamp, other vehicles 
 * that are still updating will receive valid schedules that no longer consider the invalid vehicle.
 * 
 */
TEST_F(all_stop_scheduler_test, two_vehicles_one_invalid_future_timestamp){
    // intersection_schedule schedule;
    schedule = std::make_shared<all_stop_intersection_schedule>();

    schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // Update to date vehicle
    vehicle veh;
    veh._id = "TEST01";
    veh._accel_max = 2.0;
    veh._decel_max = -1.5;
    veh._cur_speed = 4.4704;
    veh._cur_accel = 1.0;
    veh._cur_distance = 60;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp - 300;
    veh._entry_lane_id = 167;
    veh._link_id = 169;
    veh._exit_lane_id = 168;
    veh._direction = "straight";
    veh_list.insert({veh._id,veh});

    vehicle veh2;
    veh2._id = "TEST02";
    veh2._accel_max = 2.0;
    veh2._decel_max = -1.5;
    veh2._cur_speed = 4.4704;
    veh2._cur_accel = 1.0;
    veh2._cur_distance = 30;
    veh2._cur_lane_id = 167;
    veh2._cur_state = vehicle_state::EV;
    veh2._cur_time = schedule->timestamp + 10000; // This vehicle update is 10 seconds old and should no longer be considered.
    veh2._entry_lane_id = 167;
    veh2._link_id = 169;
    veh2._exit_lane_id = 168;
    veh2._direction = "straight";
    veh_list.insert({veh2._id,veh2});

    scheduler->schedule_vehicles(veh_list,schedule);
    auto sched = std::dynamic_pointer_cast<all_stop_intersection_schedule> (schedule);

    // Schedule should only contain a single vehicle an be identical to if there was only one vehicle.
    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->vehicle_schedules.front().st);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->vehicle_schedules.front().et);
    
}


/**
 * @brief Test case with single EV. Speed limit and intersection geometry should allow EV to reach speed limit and cruise 
 * shortly in both the entry lane and the link lane.
 * 
 */
TEST_F(all_stop_scheduler_test, one_vehicle_with_cruising){
    // intersection_schedule schedule;
    schedule = std::make_shared<all_stop_intersection_schedule>();

    schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    vehicle veh;
    veh._id = "TEST01";
    veh._accel_max = 2.0;
    veh._decel_max = -2.0;
    veh._cur_speed = 4.4704;
    veh._cur_accel = 0.0;
    veh._cur_distance = 60.0;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 162;
    veh._direction = "left";
    veh._departure_position = 1;
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list,schedule);
    auto sched = std::dynamic_pointer_cast<all_stop_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);
    double est_time =(sched->vehicle_schedules.front().est-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EST time for scheduler  : {0}  vs calculated {1} ", est_time, 9.169 );
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->timestamp+9169);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->vehicle_schedules.front().st);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->vehicle_schedules.front().et);
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 14.044);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp+14044);
}
/**
 * @brief Test one EV future time kinematics estimate given no possible lane transition.
 * 
 */
TEST_F(all_stop_scheduler_test, one_vehicle_with_future_time_estimate){
    // intersection_schedule schedule;
    schedule = std::make_shared<all_stop_intersection_schedule>();
    
    schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    vehicle veh;
    veh._id = "TEST01";
    veh._accel_max = 2.0;
    veh._decel_max = -2.0;
    veh._cur_speed = 4.4704;
    veh._cur_accel = 2;
    veh._cur_distance = 60.0;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp-150;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 162;
    veh._direction = "left";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list,schedule);
    auto sched = std::dynamic_pointer_cast<all_stop_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);
    auto estimate_veh = veh_list.find(veh._id)->second;
    ASSERT_EQ(estimate_veh._cur_time, sched->timestamp);
    ASSERT_NEAR(estimate_veh._cur_distance, 59.30694, 0.00001);
    ASSERT_NEAR(estimate_veh._cur_speed, 4.7704, 0.0001);

    
}

/**
 * @brief Test one EV future time kinematics estimate given possible lane transition.
 * 
 */
TEST_F(all_stop_scheduler_test, one_vehicle_lane_transition_with_future_time_estimate){
    // intersection_schedule schedule;
    schedule = std::make_shared<all_stop_intersection_schedule>();

    schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    vehicle veh;
    // NOTE: This is an impossible test scenario where the vehicle cannot stop before reaching the stop bar.
    // This test is to exercise the future time kinematic estimations where a lane transition is possible.
    veh._id = "TEST01";
    veh._accel_max = 2.0;
    veh._decel_max = -2.0;
    veh._cur_speed = 4.4704;
    veh._cur_accel = -1.5;
    veh._cur_distance = .8;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp-400;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 162;
    veh._direction = "left";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list,schedule);
    auto sched = std::dynamic_pointer_cast<all_stop_intersection_schedule> (schedule);
    
    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);
    auto estimate_veh = veh_list.find(veh._id)->second;
    ASSERT_EQ(estimate_veh._cur_time, sched->timestamp);
    ASSERT_EQ(estimate_veh._cur_distance, 0.0);
    double est_time =(sched->vehicle_schedules.front().est-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EST time for scheduler  : {0}", est_time);
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}", dt_time);

    
}

/**
 * @brief Test one EV. Kinematics and deceleration limit only allow for trajectory of deceleration at constant max deceleration.
 * 
 */
TEST_F(all_stop_scheduler_test, one_vehicle_lane_with_zero_acceleration_time){
    // intersection_schedule schedule;
    schedule = std::make_shared<all_stop_intersection_schedule>();

    schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    vehicle veh;
    // NOTE: This is a test to see if the vehicle trajectory will be calculated correctly for a vehicle which only has time to decelerate
    // with max deceleration.
    veh._id = "TEST01";
    veh._accel_max = 2.0;
    veh._decel_max = -2.0;
    veh._cur_speed = 1.0;
    veh._cur_accel = 0;
    veh._cur_distance = .25;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 162;
    veh._direction = "left";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list,schedule);
    auto sched = std::dynamic_pointer_cast<all_stop_intersection_schedule> (schedule);
    
    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);
    auto estimate_veh = veh_list.find(veh._id)->second;
    ASSERT_EQ(estimate_veh._cur_time, sched->timestamp);
    ASSERT_EQ( sched->vehicle_schedules.front().est, sched->timestamp + 500);

    
}
