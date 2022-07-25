
#include "signalized_intersection_schedule.h"

namespace streets_vehicle_scheduler {

    std::string signalized_intersection_schedule::toCSV() const {
    
        std::string schedule_info = "";
        for (const auto &sched : vehicle_schedules ){
            schedule_info += std::to_string(timestamp) + ",";
            schedule_info += sched.v_id + ",";
            schedule_info += std::to_string(sched.entry_lane) + ",";
            schedule_info += std::to_string(sched.link_id) + ",";
            schedule_info += std::to_string(sched.eet) + ",";
            schedule_info += std::to_string(sched.et) + ",";
            schedule_info += std::to_string(sched.dt) + ",";
            if ( sched.state == streets_vehicles::vehicle_state::EV) {
                schedule_info += "EV";
            }else if ( sched.state == streets_vehicles::vehicle_state::DV) {
                schedule_info += "DV";
            }else {
                schedule_info += "ND";
            }
            schedule_info += "\n";	
        }

        return schedule_info;
    }

    std::string signalized_intersection_schedule::toJson() const {
        
        rapidjson::Document doc;
        doc.SetObject();
        rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();

        rapidjson::Value metadata(rapidjson::kObjectType);
        metadata.AddMember("timestamp", timestamp, allocator);
        metadata.AddMember("intersection_type", "Carma/signalized_intersection", allocator);
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

    rapidjson::Value signalized_vehicle_schedule::toJson(rapidjson::Document::AllocatorType& allocator) const {

        rapidjson::Value vehicle_sched(rapidjson::kObjectType);
        vehicle_sched.AddMember("v_id", v_id, allocator);
        vehicle_sched.AddMember("et", et, allocator);
        vehicle_sched.AddMember("dt", dt, allocator);
        return vehicle_sched;
    }
}