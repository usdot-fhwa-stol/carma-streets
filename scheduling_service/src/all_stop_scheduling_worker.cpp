#include "all_stop_scheduling_worker.h"

namespace scheduling_service{


	bool all_stop_scheduling_worker::start_next_schedule(u_int64_t last_schedule_timestamp, u_int64_t scheduling_delta)
	{
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - std::chrono::milliseconds(last_schedule_timestamp).count() >= std::chrono::milliseconds(scheduling_delta).count())
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	streets_vehicle_scheduler::intersection_schedule all_stop_scheduling_worker::schedule_vehicles(std::unordered_map<std::string, streets_vehicles::vehicle> veh_map, std::shared_ptr<streets_vehicle_scheduler::all_stop_vehicle_scheduler> scheduler){

		streets_vehicle_scheduler::intersection_schedule int_schedule;
		int_schedule.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		scheduler->schedule_vehicles(veh_map, int_schedule);

		if ( streets_service::streets_configuration::get_boolean_config("enable_schedule_logging") ) {
			auto logger = spdlog::get("csv_logger");
			if ( logger != nullptr ){
				logger->info( int_schedule.toCSV());
			}
		}

		return int_schedule;
	}


	std::string all_stop_scheduling_worker::create_schedule_plan(streets_vehicle_scheduler::intersection_schedule int_schedule)
	{
		rapidjson::Document document;
		document.SetObject();
		rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

		rapidjson::Value metadata(rapidjson::kObjectType);
		metadata.AddMember("timestamp", int_schedule.timestamp, allocator);
		metadata.AddMember("intersection_type", "Carma/stop_controlled_intersection",allocator);
		document.AddMember("metadata", metadata, allocator);

		rapidjson::Value payload = int_schedule.toJson(allocator);    
		document.AddMember("payload", payload, allocator);

		rapidjson::StringBuffer buffer;                
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		document.Accept(writer);
		std::string msg_to_send = buffer.GetString();

		return msg_to_send;
	}


}

