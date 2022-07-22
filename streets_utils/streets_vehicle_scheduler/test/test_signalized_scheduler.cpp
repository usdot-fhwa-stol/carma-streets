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

        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            
            schedule = std::make_shared<signalized_intersection_schedule>();
            // schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            schedule->timestamp = 1000000;

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

            signal_phase_and_timing::spat spat_message;
            std::string json_spat = "{\"timestamp\":1000000,\"name\":\"West Intersection\",\"intersections\":[{\"name\":\"West Intersection\",\"id\":1909,\"revision\":123,\"status\":\"01001001001\",\"moy\":34232,\"time_stamp\":130,\"enabled_lanes\":[167,171,163],\"states\":[{\"movement_name\":\"All Directions\",\"signal_group\":1,\"state_time_speed\":[{\"event_state\":6,\"timing\":{\"start_time\":999995,\"min_end_time\":1000010}},{\"event_state\":8,\"timing\":{\"start_time\":1000010,\"min_end_time\":1000013}}, {\"event_state\":2,\"timing\":{\"start_time\":1000013,\"min_end_time\":1000030}}]},{\"movement_name\":\"All Directions\",\"signal_group\":2,\"state_time_speed\":[{\"event_state\":2,\"timing\":{\"start_time\":999995,\"min_end_time\":1000015}},{\"event_state\":6,\"timing\":{\"start_time\":1000015,\"min_end_time\":1000025}}, {\"event_state\":8,\"timing\":{\"start_time\":1000025,\"min_end_time\":1000028}}, {\"event_state\":2,\"timing\":{\"start_time\":1000028,\"min_end_time\":1000030}}]},{\"movement_name\":\"All Directions\",\"signal_group\":3,\"state_time_speed\":[{\"event_state\":2,\"timing\":{\"start_time\":999995,\"min_end_time\":1000030}}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}],\"maneuver_assist_list\":[{\"connection_id\":7,\"queue_length\":4,\"available_storage_length\":8,\"wait_on_stop\":true,\"ped_bicycle_detect\":false}]}]}";
            spat_message.fromJson(json_spat);
            std::shared_ptr<signal_phase_and_timing::spat> spat_ptr = std::make_shared<signal_phase_and_timing::spat>(spat_message);
            scheduler->set_spat(spat_ptr);
            scheduler->set_initial_green_buffer(2000);
            scheduler->set_final_green_buffer(2000);
            SPDLOG_INFO("start time = {0}", spat_ptr->intersections.front().states.front().state_time_speed.front().timing.start_time);
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
 * @brief Test case with single EV. Speed limit, acceleration/deceleration limits and intersection geometry should not allow this
 * EV to reach lanelet speed limit in entry lane or link lane. This means the vehicle never reaches a cruising speed.
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
    veh._direction = "right";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list, schedule);
    auto sched = std::dynamic_pointer_cast<signalized_intersection_schedule> (schedule);

    ASSERT_EQ( sched->vehicle_schedules.size(), 1);
    ASSERT_EQ( sched->vehicle_schedules.front().v_id, veh._id);

    double eet_time =(sched->vehicle_schedules.front().eet-sched->timestamp)/1000.0;
    SPDLOG_INFO( "EST time for scheduler  : {0}  vs calculated {1} ", eet_time, 5.291 );
    ASSERT_EQ( sched->vehicle_schedules.front().eet, sched->timestamp+5391);
    ASSERT_EQ( sched->vehicle_schedules.front().et, sched->vehicle_schedules.front().eet);
    double dt_time =(sched->vehicle_schedules.front().dt-sched->timestamp)/1000.0;
    SPDLOG_INFO( "DT time for scheduler  : {0}  vs calculated {1} ", dt_time, 9.049);
    ASSERT_EQ( sched->vehicle_schedules.front().dt, sched->timestamp+9149);

}
