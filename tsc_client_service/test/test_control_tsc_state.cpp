#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gtest/gtest_prod.h>

#include "ntcip_oids.h"
#include "control_tsc_state.h"
#include "streets_configuration.h"
#include <mock_snmp_client.h>

using testing::_;

namespace traffic_signal_controller_service
{ 

    TEST(traffic_signal_controller_service, test_control_tsc_state)
    {
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;

        auto mock_client = std::make_shared<mock_snmp_client>();

        // Define Control Type
        snmp_response_obj hold_control;
        hold_control.val_int = 255;
        hold_control.type = snmp_response_obj::response_type::INTEGER;
        
        EXPECT_CALL(*mock_client, process_snmp_request(_, _ , _) )
            .WillRepeatedly(testing::DoAll(testing::Return(true)));

        
        std::shared_ptr<mock_snmp_client> shared_client = std::move(mock_client);
        streets_desired_phase_plan::streets_desired_phase_plan desired_phase_plan;

        // Define Desired Phase plan
        using namespace std::chrono;
        system_clock clock;
        streets_desired_phase_plan::signal_group2green_phase_timing event1;
        event1.start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(1)).count();
        event1.end_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(5)).count();
        event1.signal_groups = {1,5};

        desired_phase_plan.desired_phase_plan.push_back(event1);

        streets_desired_phase_plan::signal_group2green_phase_timing event2;
        event2.start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(6)).count();;
        event2.end_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(10)).count();
        event2.signal_groups = {2,6};

        desired_phase_plan.desired_phase_plan.push_back(event2);
        auto desired_phase_plan_ptr = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>(desired_phase_plan);
        
        streets_desired_phase_plan::signal_group2green_phase_timing event3;
        event3.start_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(11)).count();;
        event3.end_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() + std::chrono::milliseconds(20)).count();
        event3.signal_groups = {7,8};
        desired_phase_plan.desired_phase_plan.push_back(event3);
        auto desired_phase_plan_ptr_2 = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>(desired_phase_plan);
        // Define Worker
        std::unordered_map<int, int> signal_group_2ped_phase_map = {{1,1}, {2,2}, {3,3}, {4,4}, {5,5}, {6,6}, {7,7}, {8,8}};
        traffic_signal_controller_service::control_tsc_state worker(shared_client, signal_group_2ped_phase_map);
        
        
        // Test update queue
        std::queue<snmp_cmd_struct> control_commands_queue;
        EXPECT_NO_THROW(worker.update_tsc_control_queue(desired_phase_plan_ptr,control_commands_queue));
        EXPECT_NO_THROW(worker.update_tsc_control_queue(desired_phase_plan_ptr_2,control_commands_queue));

        desired_phase_plan_ptr->desired_phase_plan.back().signal_groups = {1,6};
        EXPECT_THROW(worker.update_tsc_control_queue(desired_phase_plan_ptr,control_commands_queue), control_tsc_state_exception);

        // Test snmp_cmd_struct
        snmp_cmd_struct test_control_obj(shared_client, event1.start_time,snmp_cmd_struct::control_type::Hold, 0);
        EXPECT_TRUE(test_control_obj.run());

        snmp_cmd_struct test_control_obj_2(shared_client, event1.start_time,snmp_cmd_struct::control_type::Omit, 0);
        EXPECT_TRUE(test_control_obj_2.run());

        // Test empty desired phase plan
        streets_desired_phase_plan::streets_desired_phase_plan desired_phase_plan_2;
        auto dpp_ptr_2 = std::make_shared<streets_desired_phase_plan::streets_desired_phase_plan>(desired_phase_plan_2);
        EXPECT_NO_THROW(worker.update_tsc_control_queue(dpp_ptr_2,control_commands_queue));

    }

}