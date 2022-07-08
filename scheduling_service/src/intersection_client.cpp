
#include "intersection_client.h"

namespace scheduling_service {

	bool intersection_client::update_intersection_info(const int sleep_millisecs, const int int_client_request_attempts)
	{
		auto sleep_secs = static_cast<unsigned int>(sleep_millisecs/1000);
		int attempt_count = 0;
		while (attempt_count < int_client_request_attempts)
		{	
			// Send HTTP request, and update intersection information. If updated successfully, it returns true and exit the while loop.
			if (request_intersection_info())
			{
				SPDLOG_INFO("Intersection information is updated successfuly! ");
				return true;
			}
			// sleep takes seconds
			std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));
			attempt_count++;
		}
		// If failed to update the intersection information after certain numbers of attempts
		SPDLOG_ERROR("Updating Intersection information failed. ");
		return false;
	}


	bool intersection_client::request_intersection_info()
    {
		bool intersection_info_valid = false;
        OpenAPI::OAIDefaultApi apiInstance;
		OpenAPI::OAIIntersection_info int_info;
		QString error_str;
        QEventLoop loop;
        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignal, [this, &int_info, &intersection_info_valid, &loop]{     

			SPDLOG_INFO("request_intersection_info succeed!");
			SPDLOG_DEBUG("intersection name: {0}", int_info.getName().toStdString());
			SPDLOG_DEBUG("intersection id: {0}", int_info.getId());

			intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>(int_info);
			intersection_info_valid = true;

            loop.quit(); });

        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignalE, 
			[&error_str, &loop]{ 
			SPDLOG_ERROR("Error happened while issuing intersection model GET information request : {0}",  error_str.toStdString());
			loop.quit(); });

        apiInstance.getIntersectionInfo();

        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
        loop.exec();

        SPDLOG_INFO("Done with request_intersection_info");
        return intersection_info_valid;
    }


	std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_client::get_intersection_info() const 
	{
		return intersection_info_ptr;
	}


}
