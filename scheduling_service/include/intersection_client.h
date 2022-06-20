#pragma once

#include <intersection_client_api_lib/OAIHelpers.h>
#include "intersection_client_api_lib/OAIDefaultApi.h"
#include <spdlog/spdlog.h>
#include <QTimer>
#include <QEventLoop>
#include <iostream>


namespace scheduling_service{

	class intersection_client
	{
	private:


		std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info;

		
		void connect_intersection_info();

	public:

		/**
		 * @brief Get the intersection info object
		 * 
		 * @return std::shared_ptr<OpenAPI::OAIIntersection_info> 
		 */
		std::shared_ptr<OpenAPI::OAIIntersection_info> get_intersection_info();

	};

}
