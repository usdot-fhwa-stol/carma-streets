#include "intersection_schedule.h"

namespace streets_vehicle_scheduler {
    uint64_t intersection_schedule::get_delay() const{
        uint64_t delay = 0; 
        for (auto veh_sched : vehicle_schedules ) {
            delay += veh_sched.et - veh_sched.st; 
        }
        return delay;
    }

    std::string intersection_schedule::toCSV() const {
	
        std::string schedule_info = "";
        for (auto sched : vehicle_schedules ){
            schedule_info += std::to_string(timestamp) + ",";
            schedule_info += sched.v_id + ",";
            schedule_info += std::to_string(sched.entry_lane) + ",";
            schedule_info += std::to_string(sched.link_id) + ",";
            schedule_info += std::to_string(sched.dp) + ",";
            schedule_info += std::to_string(sched.est) + ",";
            schedule_info += std::to_string(sched.st) + ",";
            schedule_info += std::to_string(sched.et) + ",";
            schedule_info += std::to_string(sched.dt) + ",";
            schedule_info += (sched.access ? "true,": "false,");
            if ( sched.state == streets_vehicles::vehicle_state::DV) {
                schedule_info += "DV";
            }else if ( sched.state == streets_vehicles::vehicle_state::EV) {
                schedule_info += "EV";
            }else if ( sched.state == streets_vehicles::vehicle_state::RDV) {
                schedule_info += "RDV";
            }else {
                schedule_info += "ND";
            }
            schedule_info += "\n";	
        }

	    return schedule_info;
    }
}