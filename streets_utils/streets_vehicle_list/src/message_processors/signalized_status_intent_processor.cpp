#include "signalized_status_intent_processor.h"

namespace streets_vehicles
{
    // Override update_vehicle_state from based class all_stop_status_intent_processor
    void signalized_status_intent_processor::update_vehicle_state(vehicle &vehicle) const
    {
        SPDLOG_DEBUG("Update vehilce status with signalized status and intent processor.");
        if (vehicle._cur_state == vehicle_state::ND)
        {
            if (vehicle._cur_lane_id == vehicle._entry_lane_id)
            {
                vehicle._cur_state = vehicle_state::EV;
                SPDLOG_DEBUG("Vehicle {0} state is updated from ND to EV.", vehicle._id);
            }
            else
            {
                SPDLOG_CRITICAL("Vehicle with ND (Not Defined) STATE is NOT in entry lane {0} != {1}",
                                vehicle._entry_lane_id, vehicle._cur_lane_id);
            }
        }
        // If vehicle is currently entering vehicle, but not located in entry lane
        else if (vehicle._cur_state == vehicle_state::EV)
        {
            if (vehicle._cur_lane_id != vehicle._entry_lane_id)
            {
                vehicle._cur_state = vehicle_state::DV;
                vehicle._actual_et = vehicle._cur_time;
                SPDLOG_DEBUG("Vehicle {0} state is updated from EV to DV.", vehicle._id);                
            }
        }
        else if (vehicle._cur_state == vehicle_state::DV && vehicle._cur_lane_id == vehicle._exit_lane_id)
        {
            vehicle._cur_state = vehicle_state::LV;
            vehicle._actual_dt = vehicle._cur_time;
            SPDLOG_DEBUG("Vehicle {0} state is updated from DV to LV.", vehicle._id);
        }
    }
}