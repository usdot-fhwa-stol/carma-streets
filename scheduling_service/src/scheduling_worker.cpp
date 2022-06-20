
#include "scheduling_worker.h"

namespace streets_scheduling_worker {

	void scheduling_worker::connect_intersection_info() {
		
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


	std::shared_ptr<OpenAPI::OAIIntersection_info> scheduling_worker::get_intersection_info() {

		connect_intersection_info();

		return intersection_info;
	}


	std::shared_ptr<streets_vehicles::vehicle_list> scheduling_worker::create_veh_list() {

		std::shared_ptr<streets_vehicles::vehicle_list> veh_list;
		// Setup the related status_intent_processor
		if (streets_service::streets_configuration::get_string_config("intersection_type") == "stop_controlled_intersection"){
			veh_list->set_processor(std::make_shared<streets_vehicles::all_stop_status_intent_processor>());
			auto processor = std::dynamic_pointer_cast<streets_vehicles::all_stop_status_intent_processor>(veh_list->get_processor());
			processor->set_stopping_distance(streets_service::streets_configuration::get_double_config("stop_distance"));
			processor->set_stopping_speed(streets_service::streets_configuration::get_double_config("stop_speed"));
			processor->set_timeout(streets_service::streets_configuration::get_int_config("exp_delta"));

			return veh_list;
		}

	}


	std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduling_worker::create_scheduler() {

		if (streets_service::streets_configuration::get_string_config("intersection_type") == "stop_controlled_intersection"){
			std::shared_ptr<streets_vehicle_scheduler::all_stop_vehicle_scheduler> scheduler;
			scheduler->set_intersection_info(intersection_info);
			scheduler->set_flexibility_limit(streets_service::streets_configuration::get_int_config("flexibility_limit"));

			return scheduler;
		}

	}


}
