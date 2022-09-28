#include "signal_opt_messages_worker.h"

namespace signal_opt_service
{
    signal_opt_messages_worker::signal_opt_messages_worker()
    {
        SPDLOG_DEBUG("Construct signal_opt_messages_worker");
        
    }

    bool signal_opt_messages_worker::update_vehicle_list(const std::string &vehicle_json, 
                                                        std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr) const
    {
        if (vehicle_list_ptr)
        {
            vehicle_list_ptr->process_update(vehicle_json);
            return true;  
        }
        return false;
    }

    bool signal_opt_messages_worker::update_spat(const std::string &spat_json, std::shared_ptr<signal_phase_and_timing::spat> spat_ptr) const
    {
        if (spat_ptr)
        {
            try {
                spat_ptr->fromJson(spat_json);
                return true;
            }
            catch( const signal_phase_and_timing::signal_phase_and_timing_exception &e) {
                SPDLOG_ERROR("Exception encountered during SPaT processing! \n {0}", e.what());
                return false;
            }
        }

        return false;
    }
    bool signal_opt_messages_worker::update_tsc_config(const std::string &tsc_configuration, 
                                                       std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_configuration_ptr) const {
        if (tsc_configuration_ptr) {
            try {
                tsc_configuration_ptr->fromJson(tsc_configuration);
                return true;
            }
            catch( const streets_tsc_configuration::tsc_configuration_state_exception &e) {
                SPDLOG_ERROR("Exception encountered during TSC Configuration processing! \n {0}", e.what());
                return false;
            }
        }
        return false;
    }

    bool signal_opt_messages_worker::request_intersection_info( std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr) const
    {
        int invalid_signal_group_count = 0;
        bool signal_group_ids_valid = false;
        OpenAPI::OAIDefaultApi apiInstance;
        QEventLoop loop;
        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignal, [ intersection_info_ptr, 
                                                                                    &signal_group_ids_valid, 
                                                                                    &invalid_signal_group_count, 
                                                                                    &loop](const OpenAPI::OAIIntersection_info &int_info)
                {                   
                    SPDLOG_INFO("request_intersection_info receives intersection information. Checking signal group ids update...");
                    QList<OpenAPI::OAILanelet_info> ll_info_list = int_info.getLinkLanelets();
                    for(const auto& ll_info : ll_info_list)
                    {
                        invalid_signal_group_count += ll_info.getSignalGroupId() == 0 ? 1 : 0;
                    }  

                    if(invalid_signal_group_count == 0)
                    {
                        //Update intersection info 
                        intersection_info_ptr->setId(int_info.getId());
                        intersection_info_ptr->setName(int_info.getName());
                        intersection_info_ptr->setEntryLanelets(int_info.getEntryLanelets());
                        intersection_info_ptr->setLinkLanelets(int_info.getLinkLanelets());
                        intersection_info_ptr->setDepartureLanelets(int_info.getDepartureLanelets()); 
                        signal_group_ids_valid = true;
                        SPDLOG_INFO("Intersection information is updated with valid signal group ids! ");
                    }
                SPDLOG_INFO("Exit request_intersection_info.");
            loop.quit(); });

        connect(&apiInstance, &OpenAPI::OAIDefaultApi::getIntersectionInfoSignalE, [this, &signal_group_ids_valid, &invalid_signal_group_count, &loop](OpenAPI::OAIIntersection_info, QNetworkReply::NetworkError, QString error_str)
                { 
                    SPDLOG_ERROR("Error happened while issuing intersection model GET information request : {0}",  error_str.toStdString());
                    loop.quit(); });

        apiInstance.getIntersectionInfo();

        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
        loop.exec();
        return signal_group_ids_valid;
    }
}