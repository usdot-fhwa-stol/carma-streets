#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "vehicle_list.h"
#include "all_stop_vehicle_scheduler.h"

using namespace streets_vehicles;
using namespace streets_vehicle_scheduler;
namespace {

    class all_stop_scheduler_test : public ::testing::Test {
    private:

       

    protected:
        std::unordered_map<std::string, vehicle> veh_list;

        std::unique_ptr<all_stop_vehicle_scheduler> scheduler;

        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            scheduler = std::unique_ptr<all_stop_vehicle_scheduler>(new all_stop_vehicle_scheduler());
            scheduler->set_flexibility_limit(2);
            OpenAPI::OAIIntersection_info info;
            std::string json = "{\"departure_lanelets\":[{ \"id\":162, \"length\":41.60952439839113, \"speed_limit\":25 }, { \"id\":164, \"length\":189.44565302601367, \"speed_limit\":25 }, { \"id\":168, \"length\":34.130869420842046, \"speed_limit\":25 } ], \"entry_lanelets\":[ { \"id\":167, \"length\":195.73023157287864, \"speed_limit\":25 }, { \"id\":171, \"length\":34.130869425431136, \"speed_limit\":25 }, { \"id\":163, \"length\":41.60952435603712, \"speed_limit\":25 } ], \"id\":9001, \"link_lanelets\":[ { \"conflict_lanelet_ids\":[ 161 ], \"id\":169, \"length\":15.85409574709938, \"speed_limit\":0 }, { \"conflict_lanelet_ids\":[ 165, 156, 161 ], \"id\":155, \"length\":16.796388658952235, \"speed_limit\":0 }, { \"conflict_lanelet_ids\":[ 155, 161, 160 ], \"id\":165, \"length\":15.853947840258943, \"speed_limit\":0 }, { \"conflict_lanelet_ids\":[ 155 ], \"id\":156, \"length\":9.744590320260139, \"speed_limit\":0 }, { \"conflict_lanelet_ids\":[ 169, 155, 165 ], \"id\":161, \"length\":16.043077028554038, \"speed_limit\":0 }, { \"conflict_lanelet_ids\":[ 165 ], \"id\":160, \"length\":10.295559117055083, \"speed_limit\":0 } ], \"name\":\"WestIntersection\"}";
            info.fromJson(QString::fromStdString(json));
            std::shared_ptr<OpenAPI::OAIIntersection_info> intersection = std::make_shared<OpenAPI::OAIIntersection_info>(info);
            scheduler->set_intersection_info(intersection);

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
    intersection_schedule schedule;
    schedule.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    scheduler->schedule_vehicles(veh_list,schedule);
    ASSERT_EQ( schedule.vehicle_schedules.size(), 0);
    //  
}

TEST_F(all_stop_scheduler_test, one_vehicle){
    intersection_schedule schedule;
    schedule.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    vehicle veh;
    veh._id = "TEST01";
    veh._accel_max = 2.0;
    veh._decel_max = -2.0;
    veh._cur_speed = 4.4704;
    veh._cur_accel = 0.0;
    veh._cur_distance = 30.0;
    veh._cur_lane_id = 167;
    veh._cur_state = vehicle_state::EV;
    veh._cur_time = schedule.timestamp;
    veh._entry_lane_id = 167;
    veh._link_id = 169;
    veh._exit_lane_id = 162;
    veh._direction = "straight";
    veh_list.insert({veh._id,veh});

    scheduler->schedule_vehicles(veh_list,schedule);
    ASSERT_EQ( schedule.vehicle_schedules.size(), 1);
}
