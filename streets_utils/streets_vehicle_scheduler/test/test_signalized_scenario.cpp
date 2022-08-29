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

    class signalized_scenario_test : public ::testing::Test {
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
            std::string json_info = "{\"departure_lanelets\":[{ \"id\":162, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":164, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":168, \"length\":34.130869420842046, \"speed_limit\":11.176 } ], \"entry_lanelets\":[ { \"id\":167, \"length\":195.73023157287864, \"speed_limit\":11.176, \"signal_group_id\":1 }, { \"id\":171, \"length\":34.130869411176431136, \"speed_limit\":11.176, \"signal_group_id\":2 }, { \"id\":163, \"length\":41.60952435603712, \"speed_limit\":11.176 ,\"signal_group_id\":3 } ], \"id\":9001, \"link_lanelets\":[ { \"conflict_lanelet_ids\":[ 161 ], \"id\":169, \"length\":15.85409574709938, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 165, 156, 161 ], \"id\":155, \"length\":16.796388658952235, \"speed_limit\":4.4704 }, { \"conflict_lanelet_ids\":[ 155, 161, 160 ], \"id\":165, \"length\":15.853947840111768943, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 155 ], \"id\":156, \"length\":9.744590320260139, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 169, 155, 165 ], \"id\":161, \"length\":16.043077028554038, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 165 ], \"id\":160, \"length\":10.295559117055083, \"speed_limit\":11.176 } ], \"name\":\"WestIntersection\"}";

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

            signal_phase_and_timing::spat spat_message;
            std::string json_spat = "{\"timestamp\":0,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"status\":0,\"id\":1909,\"revision\":123,\"status\":\"01001001001\",\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[167,171,163],\"states\":[{\"movement_name\":\"All Directions\",\"signal_group\":1,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":9950,\"min_end_time\":10100}},{\"event_state\":8,\"timing\":{\"start_time\":10100,\"min_end_time\":10130}}, {\"event_state\":2,\"timing\":{\"start_time\":10130,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":2,\"state_time_speed\":[{\"event_state\":2,\"timing\":{\"start_time\":9950,\"min_end_time\":10150}},{\"event_state\":6,\"timing\":{\"start_time\":10150,\"min_end_time\":10250}}, {\"event_state\":8,\"timing\":{\"start_time\":10250,\"min_end_time\":10280}}, {\"event_state\":2,\"timing\":{\"start_time\":10280,\"min_end_time\":10300}}]},{\"movement_name\":\"All Directions\",\"signal_group\":3,\"state_time_speed\":[{\"event_state\":2,\"timing\":{\"start_time\":9950,\"min_end_time\":10300}}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
            spat_message.fromJson(json_spat);
            spat_ptr = std::make_shared<signal_phase_and_timing::spat>(spat_message);
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

/**
 * @brief Test case with one DV, 4 evs in one lane, and 2 evs in another entry lane.
 * 
 */
TEST_F(signalized_scenario_test, multiple_evs_one_dv){

    vehicle veh_dv;
    veh_dv._id = "TEST_DV_01";
    veh_dv._length = 5.0;
    veh_dv._min_gap = 2.0;
    veh_dv._reaction_time = 1.0;
    veh_dv._accel_max = 2.0;
    veh_dv._decel_max = -1.5;
    veh_dv._cur_speed = 4.4704;
    veh_dv._cur_accel = 0.0;
    veh_dv._cur_distance = 6.0;
    veh_dv._cur_lane_id = 155;
    veh_dv._cur_state = vehicle_state::DV;
    veh_dv._cur_time = schedule->timestamp;
    veh_dv._entry_lane_id = 167;
    veh_dv._link_id = 155;
    veh_dv._exit_lane_id = 168;
    veh_dv._direction = "right";
    veh_dv._actual_et = schedule->timestamp - 2000;

    vehicle veh_ev1;
    veh_ev1._id = "TEST_EV_01";
    veh_ev1._length = 5.0;
    veh_ev1._min_gap = 2.0;
    veh_ev1._reaction_time = 1.0;
    veh_ev1._accel_max = 2.0;
    veh_ev1._decel_max = -1.5;
    veh_ev1._cur_speed = 6.7056;
    veh_ev1._cur_accel = 1.0;
    veh_ev1._cur_distance = 6.0;
    veh_ev1._cur_lane_id = 167;
    veh_ev1._cur_state = vehicle_state::EV;
    veh_ev1._cur_time = schedule->timestamp;
    veh_ev1._entry_lane_id = 167;
    veh_ev1._link_id = 155;
    veh_ev1._exit_lane_id = 168;
    veh_ev1._direction = "right";

    vehicle veh_ev2;
    veh_ev2._id = "TEST_EV_02";
    veh_ev2._length = 5.0;
    veh_ev2._min_gap = 2.0;
    veh_ev2._reaction_time = 1.0;
    veh_ev2._accel_max = 2.0;
    veh_ev2._decel_max = -1.5;
    veh_ev2._cur_speed = 6.7056;
    veh_ev2._cur_accel = 1.0;
    veh_ev2._cur_distance = 15;
    veh_ev2._cur_lane_id = 167;
    veh_ev2._cur_state = vehicle_state::EV;
    veh_ev2._cur_time = schedule->timestamp;
    veh_ev2._entry_lane_id = 167;
    veh_ev2._link_id = 155;
    veh_ev2._exit_lane_id = 168;
    veh_ev2._direction = "right";

    vehicle veh_ev3;
    veh_ev3._id = "TEST_EV_03";
    veh_ev3._length = 5.0;
    veh_ev3._min_gap = 2.0;
    veh_ev3._reaction_time = 1.0;
    veh_ev3._accel_max = 2.0;
    veh_ev3._decel_max = -1.5;
    veh_ev3._cur_speed = 6.7056;
    veh_ev3._cur_accel = 1.0;
    veh_ev3._cur_distance = 40;
    veh_ev3._cur_lane_id = 167;
    veh_ev3._cur_state = vehicle_state::EV;
    veh_ev3._cur_time = schedule->timestamp;
    veh_ev3._entry_lane_id = 167;
    veh_ev3._link_id = 155;
    veh_ev3._exit_lane_id = 168;
    veh_ev3._direction = "right";

    vehicle veh_ev4;
    veh_ev4._id = "TEST_EV_04";
    veh_ev4._length = 5.0;
    veh_ev4._min_gap = 2.0;
    veh_ev4._reaction_time = 1.0;
    veh_ev4._accel_max = 2.0;
    veh_ev4._decel_max = -1.5;
    veh_ev4._cur_speed = 6.7056;
    veh_ev4._cur_accel = 1.0;
    veh_ev4._cur_distance = 77;
    veh_ev4._cur_lane_id = 167;
    veh_ev4._cur_state = vehicle_state::EV;
    veh_ev4._cur_time = schedule->timestamp;
    veh_ev4._entry_lane_id = 167;
    veh_ev4._link_id = 155;
    veh_ev4._exit_lane_id = 168;
    veh_ev4._direction = "right";

    vehicle veh_ev5;
    veh_ev5._id = "TEST_EV_05";
    veh_ev5._length = 5.0;
    veh_ev5._min_gap = 2.0;
    veh_ev5._reaction_time = 1.0;
    veh_ev5._accel_max = 2.0;
    veh_ev5._decel_max = -1.5;
    veh_ev5._cur_speed = 6.7056;
    veh_ev5._cur_accel = 1.0;
    veh_ev5._cur_distance = 30;
    veh_ev5._cur_lane_id = 171;
    veh_ev5._cur_state = vehicle_state::EV;
    veh_ev5._cur_time = schedule->timestamp;
    veh_ev5._entry_lane_id = 171;
    veh_ev5._link_id = 169;
    veh_ev5._exit_lane_id = 162;
    veh_ev5._direction = "straight";

    vehicle veh_ev6;
    veh_ev6._id = "TEST_EV_06";
    veh_ev6._length = 5.0;
    veh_ev6._min_gap = 2.0;
    veh_ev6._reaction_time = 1.0;
    veh_ev6._accel_max = 2.0;
    veh_ev6._decel_max = -1.5;
    veh_ev6._cur_speed = 6.7056;
    veh_ev6._cur_accel = 1.0;
    veh_ev6._cur_distance = 50;
    veh_ev6._cur_lane_id = 171;
    veh_ev6._cur_state = vehicle_state::EV;
    veh_ev6._cur_time = schedule->timestamp;
    veh_ev6._entry_lane_id = 171;
    veh_ev6._link_id = 169;
    veh_ev6._exit_lane_id = 162;
    veh_ev6._direction = "straight";


    veh_list.insert({{veh_dv._id, veh_dv}, {veh_ev1._id, veh_ev1}, {veh_ev2._id, veh_ev2}, {veh_ev3._id, veh_ev3}, {veh_ev4._id, veh_ev4}, {veh_ev5._id, veh_ev5}, {veh_ev6._id, veh_ev6}});

    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<signalized_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 7);
    signalized_vehicle_schedule veh_dv_schedule;
    signalized_vehicle_schedule veh_ev1_schedule;
    signalized_vehicle_schedule veh_ev2_schedule;
    signalized_vehicle_schedule veh_ev3_schedule;
    signalized_vehicle_schedule veh_ev4_schedule;
    signalized_vehicle_schedule veh_ev5_schedule;
    signalized_vehicle_schedule veh_ev6_schedule;
    for (auto veh_sched : sched->vehicle_schedules){
        if (veh_sched.v_id == veh_dv._id){
            veh_dv_schedule = veh_sched;
        }
        else if (veh_sched.v_id == veh_ev1._id){
            veh_ev1_schedule = veh_sched;
        }
        else if (veh_sched.v_id == veh_ev2._id){
            veh_ev2_schedule = veh_sched;
        }
        else if (veh_sched.v_id == veh_ev3._id){
            veh_ev3_schedule = veh_sched;
        }
        else if (veh_sched.v_id == veh_ev4._id){
            veh_ev4_schedule = veh_sched;
        }
        else if (veh_sched.v_id == veh_ev5._id){
            veh_ev5_schedule = veh_sched;
        }  
        else if (veh_sched.v_id == veh_ev6._id){
            veh_ev6_schedule = veh_sched;
        } 
    }

    double eet_time;
    double et_time;
    double dt_time;

    ASSERT_EQ(veh_dv_schedule.v_id, veh_dv._id);
    dt_time =(veh_dv_schedule.dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: DT time for scheduler {1}  vs calculated {2} ",veh_dv_schedule.v_id, dt_time, 1.343);
    ASSERT_EQ( veh_dv_schedule.dt, sched->timestamp+1343);

    ASSERT_EQ(veh_ev1_schedule.v_id, veh_ev1._id);
    eet_time =(veh_ev1_schedule.eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: EET time for scheduler {1}  vs calculated {2} ", veh_ev1_schedule.v_id, eet_time, 1.009 );
    ASSERT_EQ( veh_ev1_schedule.eet, sched->timestamp+1009);
    et_time =(veh_ev1_schedule.et-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: ET time for scheduler {1}  vs calculated {2} ", veh_ev1_schedule.v_id, et_time, 1.009 );
    ASSERT_EQ( veh_ev1_schedule.et, veh_ev1_schedule.eet);
    dt_time =(veh_ev1_schedule.dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: DT time for scheduler  : {1}  vs calculated {2} ", veh_ev1_schedule.v_id, dt_time, 4.767);
    ASSERT_EQ( veh_ev1_schedule.dt, sched->timestamp+4767);

    ASSERT_EQ(veh_ev2_schedule.v_id, veh_ev2._id);
    eet_time =(veh_ev2_schedule.eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: EET time for scheduler {1}  vs calculated {2} ", veh_ev2_schedule.v_id, eet_time, 2.429 );
    ASSERT_EQ( veh_ev2_schedule.eet, sched->timestamp+2429);
    et_time =(veh_ev2_schedule.et-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: ET time for scheduler {1}  vs calculated {2} ", veh_ev2_schedule.v_id, et_time, 1.009 + 2.566 );
    ASSERT_EQ( veh_ev2_schedule.et, sched->timestamp + 3575);
    dt_time =(veh_ev2_schedule.dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: DT time for scheduler  : {1}  vs calculated {2} ", veh_ev2_schedule.v_id, dt_time, 3.575 + 3.758);
    ASSERT_EQ( veh_ev2_schedule.dt, sched->timestamp + 7333);

    ASSERT_EQ(veh_ev3_schedule.v_id, veh_ev3._id);
    eet_time =(veh_ev3_schedule.eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: EET time for scheduler {1}  vs calculated {2} ", veh_ev3_schedule.v_id, eet_time, 5.291 );
    ASSERT_EQ( veh_ev3_schedule.eet, sched->timestamp+5291);
    et_time =(veh_ev3_schedule.et-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: ET time for scheduler {1}  vs calculated {2} ", veh_ev3_schedule.v_id, et_time, 1.009 + 2.566 + 2.566 );
    ASSERT_EQ( veh_ev3_schedule.et, sched->timestamp + 6141);
    dt_time =(veh_ev3_schedule.dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: DT time for scheduler  : {1}  vs calculated {2} ", veh_ev3_schedule.v_id, dt_time, 6.141 + 3.758);
    ASSERT_EQ( veh_ev3_schedule.dt, sched->timestamp + 9899);

    ASSERT_EQ(veh_ev4_schedule.v_id, veh_ev4._id);
    eet_time =(veh_ev4_schedule.eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: EET time for scheduler {1}  vs calculated {2} ", veh_ev4_schedule.v_id, eet_time, 8.678 );
    ASSERT_EQ( veh_ev4_schedule.eet, sched->timestamp+8678);
    et_time =(veh_ev4_schedule.et-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: ET time for scheduler {1}  vs calculated {2} ", veh_ev4_schedule.v_id, et_time, 32.000 );
    ASSERT_EQ( veh_ev4_schedule.et, sched->timestamp + 32000);
    dt_time =(veh_ev4_schedule.dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: DT time for scheduler  : {1}  vs calculated {2} ", veh_ev4_schedule.v_id, dt_time, 32.000 + 3.758);
    ASSERT_EQ( veh_ev4_schedule.dt, sched->timestamp + 35758);

    ASSERT_EQ(veh_ev5_schedule.v_id, veh_ev5._id);
    eet_time =(veh_ev5_schedule.eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: EET time for scheduler {1}  vs calculated {2} ", veh_ev5_schedule.v_id, eet_time, 3.132 );
    ASSERT_EQ( veh_ev5_schedule.eet, sched->timestamp+3132);
    et_time =(veh_ev5_schedule.et-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: ET time for scheduler {1}  vs calculated {2} ", veh_ev5_schedule.v_id, et_time, 17.000 );
    ASSERT_EQ( veh_ev5_schedule.et, sched->timestamp + 17000);
    dt_time =(veh_ev5_schedule.dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: DT time for scheduler  : {1}  vs calculated {2} ", veh_ev5_schedule.v_id, dt_time, 17.000 + 1.419);
    ASSERT_EQ( veh_ev5_schedule.dt, sched->timestamp + 18419);

    ASSERT_EQ(veh_ev6_schedule.v_id, veh_ev6._id);
    eet_time =(veh_ev6_schedule.eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: EET time for scheduler {1}  vs calculated {2} ", veh_ev6_schedule.v_id, eet_time, 4.921 );
    ASSERT_EQ( veh_ev6_schedule.eet, sched->timestamp+4921);
    et_time =(veh_ev6_schedule.et-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: ET time for scheduler {1}  vs calculated {2} ", veh_ev6_schedule.v_id, et_time, 17.000 + 1.627 );
    ASSERT_EQ( veh_ev6_schedule.et, sched->timestamp + 18627);
    dt_time =(veh_ev6_schedule.dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "Vehicle {0}: DT time for scheduler  : {1}  vs calculated {2} ", veh_ev6_schedule.v_id, dt_time, 18.627 + 1.419);
    ASSERT_EQ( veh_ev6_schedule.dt, sched->timestamp + 20046);

}



