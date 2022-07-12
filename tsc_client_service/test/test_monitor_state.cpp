#include <gtest/gtest.h>
#include <iostream>
#include "monitor_tsc_state.h"

namespace traffic_signal_controller_service
{
    TEST(test_monitor_tsc_state, test_signal_group_phase_mapping)
    {
        std::string dummy_ip = "192.168.10.10";
        int dummy_port = 601;
        snmp_client client_worker(dummy_ip, dummy_port);

        tsc_state worker(std::make_shared<snmp_client> (client_worker));
        // Get Max channels
        EXPECT_EQ(worker.get_max_channels(), 0);
        // Get vehicle phase channels - using arbitrary max channels
        int maximum_channels = 6;
        EXPECT_TRUE(worker.get_vehicle_phase_channels(maximum_channels).empty());

        std::vector<int> phase_channels = {1,2,3,4};
        worker.map_phase_and_signalgroup(phase_channels);
    }
}
