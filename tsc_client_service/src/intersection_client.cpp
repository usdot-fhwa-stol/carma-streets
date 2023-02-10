#include "intersection_client.h"
namespace traffic_signal_controller_service
{
    
    bool intersection_client::request_intersection_info() {
        bool successful = false;
        OpenAPI::OAIDefaultApi apiInstance;
        QEventLoop loop;
        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignal, [this, &successful, &loop](const OpenAPI::OAIIntersection_info &int_info){                   
            intersection_name = int_info.getName().toStdString();
            intersection_id = int_info.getId();
            successful = true;
            loop.quit(); 
        });

        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignalE, [&loop]([[maybe_unused]]const OpenAPI::OAIIntersection_info &int_info, QNetworkReply::NetworkError, QString error_str){ 
            SPDLOG_ERROR("Error happened while issuing intersection model GET information request : {0}",  error_str.toStdString());
            loop.quit();
         });

        apiInstance.getIntersectionInfo();

        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
        loop.exec();
        return successful;
    }


    std::string intersection_client::get_intersection_name() const {
        return intersection_name;
    }

    int intersection_client::get_intersection_id() const{
        return intersection_id;
    }
} // namespace traffic_signal_controller_service


 