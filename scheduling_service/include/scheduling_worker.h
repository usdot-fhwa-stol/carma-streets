#pragma once

#include <intersection_client_api_lib/OAIHelpers.h>
#include "intersection_client_api_lib/OAIDefaultApi.h"
#include <spdlog/spdlog.h>
#include <QTimer>
#include <QEventLoop>
#include <iostream>

#include "vehicle_list.h"
#include "vehicle_scheduler.h"
#include "all_stop_vehicle_scheduler.h"
#include "all_stop_status_intent_processor.h"
#include "streets_configuration.h"


namespace streets_scheduling_worker{

	class scheduling_worker
	{
	private:


		std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info;

		
		void connect_intersection_info();

	public:
		// scheduling_worker(/* args */);
		// ~scheduling_worker();

		/**
		 * @brief Get the intersection info object
		 * 
		 * @return std::shared_ptr<OpenAPI::OAIIntersection_info> 
		 */
		std::shared_ptr<OpenAPI::OAIIntersection_info> get_intersection_info();

		/**
		 * @brief Create a new vehicle list object
		 * 
		 * @return std::shared_ptr<streets_vehicles::vehicle_list>
		 */
		std::shared_ptr<streets_vehicles::vehicle_list> create_veh_list();

		/**
		 * @brief Create a new scheduler object
		 * 
		 * @return std::shared_ptr<streets_vehicles::vehicle_list>
		 */
		std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> create_scheduler();
		
	};

}
