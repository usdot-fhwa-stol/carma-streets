
#include "intersection_client.h"

namespace scheduling_service {

	void intersection_client::connect_intersection_info() {
		
		OpenAPI::OAIDefaultApi apiInstance;
		QEventLoop loop;
		QObject::connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignal, [&](OpenAPI::OAIIntersection_info int_info){
			
			SPDLOG_DEBUG("intersection name: {0}", int_info.getName().toStdString());
			SPDLOG_DEBUG("intersection id: {0}", int_info.getId());

			intersection_info = std::make_shared<OpenAPI::OAIIntersection_info>(int_info);

			loop.quit(); });

		QObject::connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignalE, [&](OpenAPI::OAIIntersection_info, QNetworkReply::NetworkError, QString error_str)
		{
			std::string error_str_print = error_str.toStdString();
			SPDLOG_CRITICAL("Error happened while issuing request : {0}", error_str_print);
			loop.quit(); });

		apiInstance.getIntersectionInfo();

		QTimer::singleShot(5000, &loop, &QEventLoop::quit);
		loop.exec();

	}


	std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_client::get_intersection_info() {

		connect_intersection_info();

		return intersection_info;
	}


}
