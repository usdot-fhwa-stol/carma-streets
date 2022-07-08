#include "scheduling_worker.h"

namespace scheduling_service{


	bool scheduling_worker::start_next_schedule(u_int64_t last_schedule_timestamp, u_int64_t scheduling_delta) const
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


	streets_vehicle_scheduler::intersection_schedule scheduling_worker::schedule_vehicles(std::unordered_map<std::string, streets_vehicles::vehicle> veh_map, 
																								std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduler) const
	{

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

}

