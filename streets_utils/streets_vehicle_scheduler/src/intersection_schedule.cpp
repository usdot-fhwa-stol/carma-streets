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

    std::string intersection_schedule::toJson() const {
        
        rapidjson::Document doc;
        doc.SetObject();
		rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();

        rapidjson::Value metadata(rapidjson::kObjectType);
		metadata.AddMember("timestamp", timestamp, allocator);
		metadata.AddMember("intersection_type", "Carma/stop_controlled_intersection", allocator);
		doc.AddMember("metadata", metadata, allocator);

        rapidjson::Value json_sched(rapidjson::kArrayType);
        for (const auto &veh_sched: vehicle_schedules ) {
            json_sched.PushBack(veh_sched.toJson(allocator), allocator);
        }
        doc.AddMember("payload", json_sched, allocator);

        rapidjson::StringBuffer buffer;                
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);
		std::string string_sched = buffer.GetString();

        return string_sched;
    }

    rapidjson::Value vehicle_schedule::toJson(rapidjson::Document::AllocatorType& allocator) const {

        rapidjson::Value vehicle_schedule(rapidjson::kObjectType);
        vehicle_schedule.AddMember("v_id", v_id, allocator);
        vehicle_schedule.AddMember("st", st, allocator);
        vehicle_schedule.AddMember("et", et, allocator);
        vehicle_schedule.AddMember("dt", dt, allocator);
        vehicle_schedule.AddMember("dp", dp, allocator);
        vehicle_schedule.AddMember("access", access, allocator);
        return vehicle_schedule;
    }
}