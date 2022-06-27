#include "signal_opt_messages_worker.h"

namespace signal_opt_service
{
    signal_opt_messages_worker::signal_opt_messages_worker()
    {
        initialize();
    }

    void signal_opt_messages_worker::initialize()
    {
        this->intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>();
        this->vehicle_list_ptr = std::make_shared<streets_vehicles::vehicle_list>();
        this->vehicle_list_ptr->set_processor(std::make_shared<streets_vehicles::signalized_status_intent_processor>());
        this->spat_ptr = std::make_shared<signal_phase_and_timing::spat>();
    }

    bool signal_opt_messages_worker::add_update_vehicle(const std::string &vehicle_json) const
    {
        if (this->vehicle_list_ptr)
        {
            this->vehicle_list_ptr->process_update(vehicle_json);
            return true;
        }
        return false;
    }

    bool signal_opt_messages_worker::update_spat(const std::string &spat_json)
    {
        if (this->spat_ptr)
        {
            rapidjson::Document doc;
            doc.Parse(spat_json.c_str());
            if (doc.HasParseError())
            {
                SPDLOG_ERROR("Error  : {0} Offset: {1} ", doc.GetParseError(), doc.GetErrorOffset());
            };
            this->spat_ptr->fromJson(doc);
            return true;
        }

        return false;
    }

    bool signal_opt_messages_worker::request_intersection_info()
    {
        int invalid_signal_group_count = 0;
        bool signal_group_ids_valid = false;
        OpenAPI::OAIDefaultApi apiInstance;
        QEventLoop loop;
        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignal, [&](OpenAPI::OAIIntersection_info int_info)
                {                   
                    SPDLOG_INFO("request_intersection_info receives intersection information. Checking signal group ids update...");
                    QList<OpenAPI::OAILanelet_info> ll_info_list = int_info.getLinkLanelets();
                    for(auto ll_info : ll_info_list)
                    {
                        invalid_signal_group_count += ll_info.getSignalGroupId() == 0 ? 1 : 0;
                    }  

                    if(invalid_signal_group_count == 0)
                    {
                        //Update intersection info 
                        this->intersection_info_ptr = std::make_shared<OpenAPI::OAIIntersection_info>(int_info); 
                        signal_group_ids_valid = true;
                        SPDLOG_INFO("Intersection information is updated with valid signal group ids! ");
                    }
                SPDLOG_INFO("Exit request_intersection_info.");
            loop.quit(); });

        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignalE, [&](OpenAPI::OAIIntersection_info, QNetworkReply::NetworkError, QString error_str)
                { 
                    SPDLOG_ERROR("Error happened while issuing intersection model GET information request : {0}",  error_str.toStdString());
                    loop.quit(); });

        apiInstance.getIntersectionInfo();

        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
        loop.exec();
        return signal_group_ids_valid;
    }

    const std::shared_ptr<OpenAPI::OAIIntersection_info> &signal_opt_messages_worker::get_intersection_info() const
    {
        return this->intersection_info_ptr;
    }

    const std::shared_ptr<streets_vehicles::vehicle_list> &signal_opt_messages_worker::get_vehicle_list() const
    {
        return this->vehicle_list_ptr;
    }

    const std::shared_ptr<signal_phase_and_timing::spat> &signal_opt_messages_worker::get_latest_spat() const
    {
        return this->spat_ptr;
    }
}