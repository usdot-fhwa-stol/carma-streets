#pragma once

#include <intersection_client_api_lib/OAIHelpers.h>
#include "intersection_client_api_lib/OAIDefaultApi.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"
#include <spdlog/spdlog.h>
#include <QTimer>
#include <QEventLoop>
#include <iostream>
#include <unistd.h>


namespace scheduling_service{

	class intersection_client: public QObject
	{
	private:

		std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr;

	public:


		/**
         * @brief Updating the intersection info.
         * @param sleep_millisecs The current thread sleep for milliseconds after each update attempt.
         * @param int_client_request_attempts The number of attempts for the loop.
         * @return boolean. True if intersection information is updated, otherwise failed to update intersection information.
         */
		bool update_intersection_info(unsigned long sleep_millisecs, unsigned long int_client_request_attempts);

		/**
         * @brief Send http GET request to intersection model at rate of configured HZ until it gets the intersection info.
         * @param int_info Http response from intersection model.
         * @return true if intersection information is updated.
         * @return false if intersection information is not updated.
         */
		bool request_intersection_info();

		/**
		 * @brief Get the intersection info object
		 * @return std::shared_ptr<OpenAPI::OAIIntersection_info> 
		 */
		std::shared_ptr<OpenAPI::OAIIntersection_info> get_intersection_info() const;

	};

}


