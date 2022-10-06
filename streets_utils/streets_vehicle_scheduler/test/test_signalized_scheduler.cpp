#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "vehicle_list.h"
#include "signalized_vehicle_scheduler.h"
#include "signalized_intersection_schedule.h"
#include "spat.h"
#include "signal_phase_and_timing_exception.h"

using namespace streets_vehicles;
using namespace streets_vehicle_scheduler;
namespace {

    class signalized_scheduler_test : public ::testing::Test {
    private:

       

    protected:
        std::unordered_map<std::string, vehicle> veh_list;

        std::unique_ptr<signalized_vehicle_scheduler> scheduler;

        std::shared_ptr<intersection_schedule> schedule;

        std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;

        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            
            schedule = std::make_shared<signalized_intersection_schedule>();
            // the schedule timestamp is set to 10000 tenths of seconds from the current hour.
            uint64_t hour_tenth_secs = 10000;
            auto tp = std::chrono::system_clock::now();
            auto duration = tp.time_since_epoch();
            auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(duration).count();
            auto epoch_start_time = hours_since_epoch * 3600 * 1000 + hour_tenth_secs * 100;
            schedule->timestamp = epoch_start_time;

            scheduler = std::unique_ptr<signalized_vehicle_scheduler>(new signalized_vehicle_scheduler());

            OpenAPI::OAIIntersection_info info;
            std::string json_info = "{\"departure_lanelets\":[{ \"id\":162, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":164, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":168, \"length\":34.130869420842046, \"speed_limit\":11.176 } ], \"entry_lanelets\":[ { \"id\":167, \"length\":195.73023157287864, \"speed_limit\":11.176, \"connecting_lanelet_ids\": [155, 169] }, { \"id\":171, \"length\":34.130869411176431136, \"speed_limit\":11.176, \"connecting_lanelet_ids\": [160, 161] }, { \"id\":163, \"length\":41.60952435603712, \"speed_limit\":11.176 , \"connecting_lanelet_ids\": [156, 165]} ], \"id\":9001, \"link_lanelets\":[{ \"conflict_lanelet_ids\":[ 161 ], \"id\":169, \"length\":15.85409574709938, \"speed_limit\":11.176, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 165, 156, 161 ], \"id\":155, \"length\":16.796388658952235, \"speed_limit\":4.4704, \"signal_group_id\":1 }, { \"conflict_lanelet_ids\":[ 155, 161, 160 ], \"id\":165, \"length\":15.853947840111768943, \"speed_limit\":11.176, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 155 ], \"id\":156, \"length\":9.744590320260139, \"speed_limit\":11.176, \"signal_group_id\":3 }, { \"conflict_lanelet_ids\":[ 169, 155, 165 ], \"id\":161, \"length\":16.043077028554038, \"speed_limit\":11.176, \"signal_group_id\":2 }, { \"conflict_lanelet_ids\":[ 165 ], \"id\":160, \"length\":10.295559117055083, \"speed_limit\":11.176, \"signal_group_id\":2 } ], \"name\":\"WestIntersection\"}";

            info.fromJson(QString::fromStdString(json_info));
            std::shared_ptr<OpenAPI::OAIIntersection_info> intersection = std::make_shared<OpenAPI::OAIIntersection_info>(info);
            scheduler->set_intersection_info(intersection);
            SPDLOG_INFO("Intersection information consists of {0} entry lanes {1} linking lanes and {2} departure lanes.", 
                        intersection->getEntryLanelets().size(),
                        intersection->getLinkLanelets().size(), 
                        intersection->getDepartureLanelets().size()) ;

            /** The current phase in the modified spat starts from 9950 tenths of seconds from the current hour and the last future phase
             * in the modified spat ends at 10300 tenths of seconds from the current hour.
             * Signal group 1 is green from 9950 to 10100, is yellow from 10100 to 10130, and is red from 10130 to 10300.
             * Signal group 2 is red from 9950 to 10150, is green from 10150 to 10250, is yellow from 10250 to 10280, and is red 
             *      again from 10280 to 10300.
             * Signal group 3 is red from 9950 to 10300. 
             */

            std::string json_spat = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"status\":0,\"revision\":123,\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[155,156,160,161,165,169],\"states\":[{\"movement_name\":\"All Directions\",\"signal_group\":1,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}},{\"event_state\":8,\"timing\":{\"start_time\":10100,\"min_end_time\":10130}}, {\"event_state\":3,\"timing\":{\"start_time\":10130,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":2,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10150}},{\"event_state\":6,\"timing\":{\"start_time\":10150,\"min_end_time\":10250}}, {\"event_state\":8,\"timing\":{\"start_time\":10250,\"min_end_time\":10280}}, {\"event_state\":3,\"timing\":{\"start_time\":10280,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":3,\"state_time_speed\":[{\"event_state\":3,\"timing\":{\"start_time\":9950,\"min_end_time\":10300}}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
            spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
            spat_ptr->fromJson(json_spat);

            scheduler->set_spat(spat_ptr);
            scheduler->set_initial_green_buffer(2000);
            scheduler->set_final_green_buffer(2000);
        }
            
        /**
         * @brief Test TearDown method run after each test.
         * 
         */
        void TearDown() override{
            
        }
    };
};


TEST_F(signalized_scheduler_test, schedule_empty_vehicle_list) {
    
    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<signalized_intersection_schedule> (schedule);
    ASSERT_EQ( sched->vehicle_schedules.size(), 0); 
}

/**
 * @brief Test case with single DV.
 * This test checks if the departure time (DT) of a Departing Vehicle (DV) is calculated correctly. 
 * 
 */
TEST_F(signalized_scheduler_test, one_dv){

    vehicle veh;
    veh._id = "TEST01";
    veh._length = 5.0;
    veh._min_gap = 2.0;
    veh._reaction_time = 1.0;
    veh._accel_max = 2.0;
    veh._decel_max = -1.5;
    veh._cur_speed = 4.4704;
    veh._cur_accel = 0.0;
    veh._cur_distance = 6.0;
    veh._cur_lane_id = 155;
    veh._cur_state = vehicle_state::DV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 168;
    veh._direction = "left";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<signalized_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);

    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 1.343);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp+1343);

}


/**
 * @brief Test case with single EV.  (delta_x < delta_x_zegond < delta_x_prime) (eet within green and et = eet)
 * Speed limit, acceleration/deceleration limits, vehicle's current distance, and intersection geometry should allow this EV to reach 
 * the entering speed. This means that the vehicle trajectory will include one piece of acceleration\deceleration and its actual 
 * entering speed will be slightly lower\higher than the desired entering speed. 
 * Also, the vehicle's estimated EET is within a green phase and therefore, its estimated ET shall be equal to its estimated EET.
 * 
 */
TEST_F(signalized_scheduler_test, one_ev_one_decelration){

    vehicle veh;
    veh._id = "TEST01";
    veh._length = 5.0;
    veh._min_gap = 2.0;
    veh._reaction_time = 1.0;
    veh._accel_max = 2.0;
    veh._decel_max = -1.5;
    veh._cur_speed = 6.7056;
    veh._cur_accel = 1.0;
    veh._cur_distance = 6;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 168;
    veh._direction = "left";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<signalized_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);

    SPDLOG_INFO("schedule timestamp = {0}, eet = {1}, et = {2} ", sched->timestamp, sched->vehicle_schedules.front().eet, sched->vehicle_schedules.front().et);
    SPDLOG_INFO(" spat first phase start time = {0}, spat first phase end time = {1} ", 
        spat_ptr->get_intersection().states.front().state_time_speed.front().timing.get_epoch_start_time(),
        spat_ptr->get_intersection().states.front().state_time_speed.front().timing.get_epoch_min_end_time());

    double eet_time =(sched->vehicle_schedules.front().eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EET time for scheduler  : {0}  vs calculated {1} ", eet_time, 1.009 );
    ASSERT_EQ( sched->vehicle_schedules.front().eet, sched->timestamp+1009);
    ASSERT_EQ( sched->vehicle_schedules.front().et, sched->vehicle_schedules.front().eet);
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 4.767);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp+4767);

}


/**
 * @brief Test case with single EV.  (delta_x_zegond < delta_x < delta_x_prime) (eet within green and et = eet)
 * Speed limit, acceleration/deceleration limits, vehicle's current distance, and intersection geometry should allow this
 * EV to reach a speed higher than max(current speed, entering speed) before slowing down\speeding up to the entering speed. This means
 * the vehicle trajectory will include two pieces of acceleration and deceleration.
 * But it should not allow this EV to reach lanelet speed limit in entry lane. This means the vehicle trajectory will not include cruising.
 * Also, the vehicle's estimated EET is within a green phase and therefore, its estimated ET shall be equal to its estimated EET.
 * 
 */
TEST_F(signalized_scheduler_test, one_ev_without_cruising){

    vehicle veh;
    veh._id = "TEST01";
    veh._length = 5.0;
    veh._min_gap = 2.0;
    veh._reaction_time = 1.0;
    veh._accel_max = 2.0;
    veh._decel_max = -1.5;
    veh._cur_speed = 6.7056;
    veh._cur_accel = 1.0;
    veh._cur_distance = 40;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 168;
    veh._direction = "left";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<signalized_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);

    SPDLOG_INFO("schedule timestamp = {0}, eet = {1}, et = {2} ", sched->timestamp, sched->vehicle_schedules.front().eet, sched->vehicle_schedules.front().et);
    SPDLOG_INFO(" spat first phase start time = {0}, spat first phase end time = {1} ", 
        spat_ptr->get_intersection().states.front().state_time_speed.front().timing.get_epoch_start_time(), 
        spat_ptr->get_intersection().states.front().state_time_speed.front().timing.get_epoch_min_end_time());

    double eet_time =(sched->vehicle_schedules.front().eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EET time for scheduler  : {0}  vs calculated {1} ", eet_time, 5.291 );
    ASSERT_EQ( sched->vehicle_schedules.front().eet, sched->timestamp+5291);
    ASSERT_EQ( sched->vehicle_schedules.front().et, sched->vehicle_schedules.front().eet);
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 9.049);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp+9049);

}


/**
 * @brief Test case with single EV.  (delta_x_zegond < delta_x_prime < delta_x) 
 * (eet NOT within green and et = begining of TBD + initial_green_buffer)
 * Speed limit, acceleration/deceleration limits, vehicle's current distance, and intersection geometry should allow this
 * EV to reach lanelet speed limit in entry lane. This means the vehicle trajectory will include two pieces of acceleration and deceleration
 * and one piece of cruising.
 * Also, the vehicle's estimated EET is NOT within a green phase and therefore, its estimated ET shall be equal to beginning of the next 
 * green phase (TBD in this example) plus the initial_green_buffer.
 * 
 */
TEST_F(signalized_scheduler_test, one_ev_with_cruising_1){

    vehicle veh;
    veh._id = "TEST01";
    veh._length = 5.0;
    veh._min_gap = 2.0;
    veh._reaction_time = 1.0;
    veh._accel_max = 2.0;
    veh._decel_max = -1.5;
    veh._cur_speed = 6.7056;
    veh._cur_accel = 1.0;
    veh._cur_distance = 100;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 168;
    veh._direction = "left";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<signalized_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);

    SPDLOG_INFO("schedule timestamp = {0}, eet = {1}, et = {2} ", sched->timestamp, sched->vehicle_schedules.front().eet, sched->vehicle_schedules.front().et);
    SPDLOG_INFO(" spat first phase start time = {0}, spat first phase end time = {1} ", 
        spat_ptr->get_intersection().states.front().state_time_speed.front().timing.get_epoch_start_time(),
        spat_ptr->get_intersection().states.front().state_time_speed.front().timing.get_epoch_min_end_time());

    double eet_time =(sched->vehicle_schedules.front().eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EET time for scheduler  : {0}  vs calculated {1} ", eet_time, 10.736 );
    ASSERT_EQ( sched->vehicle_schedules.front().eet, sched->timestamp+10736);
    double et_time = (sched->vehicle_schedules.front().et-sched->timestamp)/1000.0;
    SPDLOG_INFO( "ET time for scheduler  : {0}  vs calculated {1} ", et_time, 32.000 );
    ASSERT_EQ( sched->vehicle_schedules.front().et, sched->timestamp + 30000 + scheduler->get_initial_green_buffer());
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 35.758);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp + 30000 + scheduler->get_initial_green_buffer() + 3758);

}


/**
 * @brief Test case with single EV.  (delta_x_zegond < delta_x_prime < delta_x) 
 * (eet within final_green_buffer of the green green and et = begining of TBD + initial_green_buffer)
 * Speed limit, acceleration/deceleration limits, vehicle's current distance, and intersection geometry should allow this
 * EV to reach lanelet speed limit in entry lane. This means the vehicle trajectory will include two pieces of acceleration and deceleration
 * and one piece of cruising.
 * Also, the vehicle's estimated EET is within final_green_buffer of the green phase and therefore, its estimated ET shall be equal to 
 * beginning of the next green phase (TBD in this example) plus the initial_green_buffer.
 * 
 */
TEST_F(signalized_scheduler_test, one_ev_with_cruising_2){

    vehicle veh;
    veh._id = "TEST01";
    veh._length = 5.0;
    veh._min_gap = 2.0;
    veh._reaction_time = 1.0;
    veh._accel_max = 2.0;
    veh._decel_max = -1.5;
    veh._cur_speed = 6.7056;
    veh._cur_accel = 1.0;
    veh._cur_distance = 77;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule->timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 155;
    veh._exit_lane_id = 168;
    veh._direction = "left";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<signalized_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);

    SPDLOG_INFO("schedule timestamp = {0}, eet = {1}, et = {2} ", sched->timestamp, sched->vehicle_schedules.front().eet, sched->vehicle_schedules.front().et);
    SPDLOG_INFO(" spat first phase start time = {0}, spat first phase end time = {1} ", 
        spat_ptr->get_intersection().states.front().state_time_speed.front().timing.get_epoch_start_time(), 
        spat_ptr->get_intersection().states.front().state_time_speed.front().timing.get_epoch_min_end_time());

    double eet_time =(sched->vehicle_schedules.front().eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EET time for scheduler  : {0}  vs calculated {1} ", eet_time, 8.678 );
    ASSERT_EQ( sched->vehicle_schedules.front().eet, sched->timestamp+8678);
    double et_time = (sched->vehicle_schedules.front().et-sched->timestamp)/1000.0;
    SPDLOG_INFO( "ET time for scheduler  : {0}  vs calculated {1} ", et_time, 32.000 );
    ASSERT_EQ( sched->vehicle_schedules.front().et, sched->timestamp + 30000 + scheduler->get_initial_green_buffer());
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 35.758);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp + 30000 + scheduler->get_initial_green_buffer() + 3758);

}



