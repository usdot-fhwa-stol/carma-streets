#define RAPIDJSON_HAS_STDSTRING 1

#include "intersection_schedule.h"

namespace streets_vehicle_scheduler {
    uint64_t intersection_schedule::get_delay() const{
        uint64_t delay = 0; 
        for (const auto &veh_sched : vehicle_schedules ) {
            delay += veh_sched.et - veh_sched.st; 
        }
        return delay;
    }

    std::string intersection_schedule::toCSV() const {
	
        std::string schedule_info = "";
        for (const auto &sched : vehicle_schedules ){
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

    rapidjson::Value intersection_schedule::toJson() const {
        
        rapidjson::Document doc;
        rapidjson::Value json_sched(rapidjson::kArrayType);
        auto allocator = doc.GetAllocator();
        for (const auto &veh_sched: vehicle_schedules ) {
            rapidjson::Value vehicle_schedule(rapidjson::kObjectType);
            vehicle_schedule.AddMember("v_id", veh_sched.v_id, allocator);
            vehicle_schedule.AddMember("st", veh_sched.st, allocator);
            vehicle_schedule.AddMember("et", veh_sched.et, allocator);
            vehicle_schedule.AddMember("dt", veh_sched.dt, allocator);
            vehicle_schedule.AddMember("dp", veh_sched.dp, allocator);
            vehicle_schedule.AddMember("access", veh_sched.access, allocator);
            json_sched.PushBack(vehicle_schedule,allocator);
        }
        return json_sched;
    }
}