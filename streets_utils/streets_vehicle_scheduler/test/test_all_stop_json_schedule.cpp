#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <boost/range/algorithm/count.hpp>

#include "vehicle_list.h"
#include "all_stop_vehicle_scheduler.h"

using namespace streets_vehicles;
using namespace streets_vehicle_scheduler;
namespace {

    class all_stop_json_schedule_test : public ::testing::Test {
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


/**
 * @brief Test with multiple vehicle for json schedule.
 */
TEST_F(all_stop_json_schedule_test, json_schedule){
    intersection_schedule schedule;
    schedule.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    vehicle veh_dv;
    veh_dv._id = "TEST_DV_01";
    veh_dv._accel_max = 2.0;
    veh_dv._decel_max = -2.0;
    veh_dv._cur_speed = 1.0;
    veh_dv._cur_accel = 2.0;
    veh_dv._cur_distance = 9.3;
    veh_dv._cur_lane_id = 160;
    veh_dv._cur_state = vehicle_state::DV;
    veh_dv._cur_time = schedule.timestamp;
    veh_dv._entry_lane_id = 158;
    veh_dv._link_id = 160;
    veh_dv._exit_lane_id = 164;
    veh_dv._direction = "right";
    veh_dv._departure_position = 1;
    veh_dv._access = true;
    veh_dv._actual_st = schedule.timestamp - 3000;
    veh_dv._actual_et = schedule.timestamp - 1000;

    vehicle veh_rdv1;
    veh_rdv1._id = "TEST_RDV_01";
    veh_rdv1._accel_max = 2.0;
    veh_rdv1._decel_max = -2.0;
    veh_rdv1._cur_speed = 0.0;
    veh_rdv1._cur_accel = 0.0;
    veh_rdv1._cur_distance = 1.0;
    veh_rdv1._cur_lane_id = 171;
    veh_rdv1._cur_state = vehicle_state::RDV;
    veh_rdv1._cur_time = schedule.timestamp;
    veh_rdv1._entry_lane_id = 171;
    veh_rdv1._link_id = 165;
    veh_rdv1._exit_lane_id = 164;
    veh_rdv1._direction = "straight";
    veh_rdv1._departure_position = 2;
    veh_rdv1._actual_st = schedule.timestamp - 1000;

    vehicle veh_rdv2;
    veh_rdv2._id = "TEST_RDV_02";
    veh_rdv2._accel_max = 2.0;
    veh_rdv2._decel_max = -2.0;
    veh_rdv2._cur_speed = 0.0;
    veh_rdv2._cur_accel = 0.0;
    veh_rdv2._cur_distance = 1.0;
    veh_rdv2._cur_lane_id = 167;
    veh_rdv2._cur_state = vehicle_state::RDV;
    veh_rdv2._cur_time = schedule.timestamp;
    veh_rdv2._entry_lane_id = 167;
    veh_rdv2._link_id = 169;
    veh_rdv2._exit_lane_id = 168;
    veh_rdv2._direction = "straight";
    veh_rdv2._departure_position = 3;
    veh_rdv2._actual_st = schedule.timestamp - 1000;

    veh_list.insert({{veh_dv._id, veh_dv}, {veh_rdv1._id, veh_rdv1}, {veh_rdv2._id, veh_rdv2}});

    scheduler->schedule_vehicles(veh_list, schedule);
    ASSERT_EQ( schedule.vehicle_schedules.size(), 3);
    rapidjson::Value json_schedule = schedule.toJson();

    ASSERT_EQ( json_schedule.Size(), 3);
    ASSERT_TRUE ( json_schedule.IsArray() );
    int new_lines = boost::count( schedule.toCSV(), '\n');
    ASSERT_EQ( new_lines, 3);

}
