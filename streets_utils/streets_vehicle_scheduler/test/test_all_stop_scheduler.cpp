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
        std::unique_ptr<vehicle_list> veh_list;

        std::unique_ptr<all_stop_vehicle_scheduler> scheduler;

        /**
         * @brief Test Setup method run before each test.
         * 
         */
        void SetUp() override {
            veh_list = std::unique_ptr<vehicle_list>(new vehicle_list());
            scheduler = std::unique_ptr<all_stop_vehicle_scheduler>(new all_stop_vehicle_scheduler());
        }
            
        /**
         * @brief Test TearDown method run after each test.
         * 
         */
        void TearDown() override{
            
        }
    };
};

TEST_F(all_stop_scheduler_test, initialize) {
    //
}
