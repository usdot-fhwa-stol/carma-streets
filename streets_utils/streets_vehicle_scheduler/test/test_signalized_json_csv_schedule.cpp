#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <boost/range/algorithm/count.hpp>

#include "vehicle_list.h"
#include "signalized_vehicle_scheduler.h"

using namespace streets_vehicles;
using namespace streets_vehicle_scheduler;
namespace {
    /**
     * @brief Test class to test signalized_intersection_schedule toCSV and toJSON methods.
     * 
     */
    class signalized_json_csv_schedule_test : public ::testing::Test {
    private:

       

    protected:
        std::unordered_map<std::string, vehicle> veh_list;

        std::unique_ptr<signalized_vehicle_scheduler> scheduler;

        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            scheduler = std::unique_ptr<signalized_vehicle_scheduler>(new signalized_vehicle_scheduler());
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
TEST_F(signalized_json_csv_schedule_test, json_schedule){
    signalized_intersection_schedule schedule;
    schedule.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // populated schedule 
    signalized_vehicle_schedule sched1;
    sched1.state =streets_vehicles::vehicle_state::DV;
    sched1.entry_lane = 1;
    sched1.link_id = 2;
    sched1.eet = 0;
    sched1.et = schedule.timestamp - 2000;

    signalized_vehicle_schedule sched2;
    sched2.state =streets_vehicles::vehicle_state::EV;
    sched2.entry_lane = 1;
    sched2.link_id = 2;
    sched2.eet = schedule.timestamp + 2000;
    sched2.et = schedule.timestamp + 2000;

    signalized_vehicle_schedule sched3;
    sched3.state =streets_vehicles::vehicle_state::EV;
    sched3.entry_lane = 1;
    sched3.link_id = 2;
    sched3.eet = schedule.timestamp + 4000;
    sched3.et =  schedule.timestamp + 6000;

    signalized_vehicle_schedule sched4;
    sched4.state =streets_vehicles::vehicle_state::EV;
    sched4.entry_lane = 3;
    sched4.link_id = 4;
    sched4.eet = schedule.timestamp + 3000;
    sched4.et =  schedule.timestamp + 10000;

    schedule.vehicle_schedules.push_back(sched1);
    schedule.vehicle_schedules.push_back(sched2);
    schedule.vehicle_schedules.push_back(sched3);
    schedule.vehicle_schedules.push_back(sched4);

    ASSERT_EQ( schedule.vehicle_schedules.size(), 4);


    std::string str_schedule = schedule.toJson();
    rapidjson::Document json_sched;
    json_sched.SetObject();
    json_sched.Parse(str_schedule.c_str());

    ASSERT_TRUE( json_sched.HasMember("metadata") );
    ASSERT_TRUE( json_sched["metadata"].HasMember("timestamp") );
    ASSERT_TRUE( json_sched["metadata"].HasMember("intersection_type") );

    ASSERT_TRUE( json_sched.HasMember("payload") );
    ASSERT_TRUE( json_sched["payload"].IsArray() );
    ASSERT_EQ( json_sched["payload"].Size(), 3);

}

/**
 * @brief Test with multiple vehicle for csv schedule.
 */
TEST_F(signalized_json_csv_schedule_test, csv_schedule){
    signalized_intersection_schedule schedule;
    schedule.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // populated schedule 
    signalized_vehicle_schedule sched1;
    sched1.state =streets_vehicles::vehicle_state::DV;
    sched1.entry_lane = 1;
    sched1.link_id = 2;
    sched1.eet = 0;
    sched1.et = schedule.timestamp - 2000;

    signalized_vehicle_schedule sched2;
    sched2.state =streets_vehicles::vehicle_state::EV;
    sched2.entry_lane = 1;
    sched2.link_id = 2;
    sched2.eet = schedule.timestamp + 2000;
    sched2.et = schedule.timestamp + 2000;

    signalized_vehicle_schedule sched3;
    sched3.state =streets_vehicles::vehicle_state::EV;
    sched3.entry_lane = 1;
    sched3.link_id = 2;
    sched3.eet = schedule.timestamp + 4000;
    sched3.et =  schedule.timestamp + 6000;

    signalized_vehicle_schedule sched4;
    sched4.state =streets_vehicles::vehicle_state::EV;
    sched4.entry_lane = 3;
    sched4.link_id = 4;
    sched4.eet = schedule.timestamp + 3000;
    sched4.et =  schedule.timestamp + 10000;

    schedule.vehicle_schedules.push_back(sched1);
    schedule.vehicle_schedules.push_back(sched2);
    schedule.vehicle_schedules.push_back(sched3);
    schedule.vehicle_schedules.push_back(sched4);

    int new_lines = boost::count( schedule.toCSV(), '\n');
    //New line after each schedule
    ASSERT_EQ( new_lines, 3);

}
