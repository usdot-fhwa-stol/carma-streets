
#pragma once

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
#include "all_stop_vehicle_scheduler.h"
#include "all_stop_status_intent_processor.h"

namespace scheduling_service{

	class all_stop_scheduling_worker
	{

	public:
		
		/**
		 * @brief Checks if the time for starting the next schedule has been reached.
		 * @param last_schedule_timestamp The timestamp of the last previous schedule.
		 * @param scheduling_delta Time(in seconds) interval between scheduling calculations.
		 * @return true if the time for starting the next schedule has been reached.
		 * @return false if the time for starting the next schedule has not been reached.
		 */
		bool start_next_schedule(u_int64_t last_schedule_timestamp, u_int64_t scheduling_delta);


		/**
		 * @brief Runs the scheduler's schedule_veh method to schedule all vehicles.
		 * @param veh_list The vehicle list object.
		 * @param scheduler The scheduler object.
		 * @return An intersection schedule object that contains vehicles' estimated critical time points.
		 */
		streets_vehicle_scheduler::intersection_schedule schedule_vehicles(std::unordered_map<std::string, streets_vehicles::vehicle> veh_map, std::shared_ptr<streets_vehicle_scheduler::all_stop_vehicle_scheduler> scheduler);

		/**
		 * @brief Create a schedule plan to be sent to the vehicles.
		 * @param int_schedule An intersection schedule object that contains vehicles' estimated critical time points.
		 * @return A json string of the schedule plan.
		 */
		std::string create_schedule_plan(streets_vehicle_scheduler::intersection_schedule int_schedule);


	};

}

