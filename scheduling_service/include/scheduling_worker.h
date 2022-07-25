
#pragma once

#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>

#include "kafka_client.h"
#include "streets_configuration.h"
#include "intersection_client.h"
#include "vehicle_list.h"
#include "vehicle_scheduler.h"
#include "all_stop_vehicle_scheduler.h"
#include "all_stop_status_intent_processor.h"
#include "signalized_vehicle_scheduler.h"
#include "signalized_status_intent_processor.h"

namespace scheduling_service{

	class scheduling_worker
	{

	public:
		

		/**
		 * @brief Runs the scheduler's schedule_veh method to schedule all vehicles.
		 * @param veh_list The vehicle list object.
		 * @param scheduler The scheduler object.
		 * @return An intersection schedule object that contains vehicles' estimated critical time points.
		 */
		std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> schedule_vehicles(std::unordered_map<std::string, streets_vehicles::vehicle> veh_map, 
																			std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduler) const;


	};

}

