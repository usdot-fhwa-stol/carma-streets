#include "IntersectionModelHandler.h"

namespace IntersectionModelAPI
{
    IntersectionModelHandler::IntersectionModelHandler(std::shared_ptr<intersection_model::intersection_model> int_worker) : int_worker_(int_worker)
    {
        SPDLOG_DEBUG("IntersectionModelHandler constructor called");
    }

    void IntersectionModelHandler::getIntersectionInfo()
    {
        auto reqObj = qobject_cast<OpenAPI::OAIDefaultApiRequest *>(sender());
        if (reqObj != nullptr)
        {
            OpenAPI::OAIIntersection_info res;
            res.setId(int_worker_->get_intersection_info().intersection_id);
            res.setName(int_worker_->get_intersection_info().intersection_name.c_str());
            QList<OpenAPI::OAILanelet_info> departure_lanlets;
            intersection_model::intersection_info_t int_info = int_worker_->get_intersection_info();
            for (std::set<intersection_model::lanelet_info_t>::iterator itr = int_info.departure_lanelets.begin(); itr != int_info.departure_lanelets.end(); itr++)
            {
                OpenAPI::OAILanelet_info lanelet_info;
                lanelet_info.setId(itr->id);
                lanelet_info.setSpeedLimit(itr->speed_limit);
                lanelet_info.setLength(itr->length);
                departure_lanlets.push_back(lanelet_info);
            }
            res.setDepartureLanelets(departure_lanlets);

            QList<OpenAPI::OAILanelet_info> link_lanlets;
            for (std::vector<intersection_model::lanelet_info_t>::iterator itr = int_info.link_lanelets.begin(); itr != int_info.link_lanelets.end(); itr++)
            {
                OpenAPI::OAILanelet_info lanelet_info;
                lanelet_info.setId(itr->id);
                lanelet_info.setSpeedLimit(itr->speed_limit);

                // Update the llink lanelet_info with the list of conflict lanelets
                std::vector<intersection_model::lanelet_info_t> conflict_lanelet_info_v = int_worker_->get_conflict_lanelets(itr->id);
                QList<qint32> conflict_lanelet_ids;
                for (auto conflict_itr = conflict_lanelet_info_v.begin(); conflict_itr != conflict_lanelet_info_v.end(); conflict_itr++)
                {
                    conflict_lanelet_ids.push_back(conflict_itr->id);
                }
                lanelet_info.setConflictLaneletIds(conflict_lanelet_ids);
                lanelet_info.setLength(itr->length);
                link_lanlets.push_back(lanelet_info);
            }
            res.setLinkLanelets(link_lanlets);

            QList<OpenAPI::OAILanelet_info> entry_lanlets;
            for (std::vector<intersection_model::lanelet_info_t>::iterator itr = int_info.entering_lanelets.begin(); itr != int_info.entering_lanelets.end(); itr++)
            {
                OpenAPI::OAILanelet_info lanelet_info;
                lanelet_info.setId(itr->id);
                lanelet_info.setSpeedLimit(itr->speed_limit);
                lanelet_info.setLength(itr->length);
                entry_lanlets.push_back(lanelet_info);
            }
            res.setEntryLanelets(entry_lanlets);
            reqObj->getIntersectionInfoResponse(res);
        }
    }

    void IntersectionModelHandler::listDepartureLanelets()
    {
        auto reqObj = qobject_cast<OpenAPI::OAIDefaultApiRequest *>(sender());
        if (reqObj != nullptr)
        {
            QList<OpenAPI::OAILanelet_info> res;
            std::set<intersection_model::lanelet_info_t> departure_lanelet_v = int_worker_->get_departure_lanelets();
            for (std::set<intersection_model::lanelet_info_t>::iterator itr = departure_lanelet_v.begin(); itr != departure_lanelet_v.end(); itr++)
            {
                OpenAPI::OAILanelet_info lanelet_info;
                lanelet_info.setId(itr->id);
                lanelet_info.setSpeedLimit(itr->speed_limit);
                lanelet_info.setLength(itr->length);
                res.push_back(lanelet_info);
            }
            reqObj->listDepartureLaneletsResponse(res);
        }
    }

    void IntersectionModelHandler::listEntryLanelets()
    {
        auto reqObj = qobject_cast<OpenAPI::OAIDefaultApiRequest *>(sender());
        if (reqObj != nullptr)
        {
            QList<OpenAPI::OAILanelet_info> res;
            std::vector<intersection_model::lanelet_info_t> entry_lanelet_v = int_worker_->get_entry_lanelets();
            for (std::vector<intersection_model::lanelet_info_t>::iterator itr = entry_lanelet_v.begin(); itr != entry_lanelet_v.end(); itr++)
            {
                OpenAPI::OAILanelet_info lanelet_info;
                lanelet_info.setId(itr->id);
                lanelet_info.setSpeedLimit(itr->speed_limit);
                lanelet_info.setLength(itr->length);
                res.push_back(lanelet_info);
            }
            reqObj->listEntryLaneletsResponse(res);
        }
    }

    void IntersectionModelHandler::listLinkLanelets()
    {
        auto reqObj = qobject_cast<OpenAPI::OAIDefaultApiRequest *>(sender());
        if (reqObj != nullptr)
        {
            QList<OpenAPI::OAILanelet_info> res;
            std::vector<intersection_model::lanelet_info_t> link_lanelet_v = int_worker_->get_link_lanelets();
            for (std::vector<intersection_model::lanelet_info_t>::iterator itr = link_lanelet_v.begin(); itr != link_lanelet_v.end(); itr++)
            {
                OpenAPI::OAILanelet_info lanelet_info;
                lanelet_info.setId(itr->id);
                lanelet_info.setSpeedLimit(itr->speed_limit);
                lanelet_info.setLength(itr->length);
                res.push_back(lanelet_info);
            }
            reqObj->listLinkLaneletsResponse(res);
        }
    }

    void IntersectionModelHandler::getConflictLanelets(qint32 link_lanelet_id)
    {
        Q_UNUSED(link_lanelet_id);
        auto reqObj = qobject_cast<OpenAPI::OAIDefaultApiRequest *>(sender());
        if (reqObj != nullptr)
        {
            QList<OpenAPI::OAILanelet_info> res;
            // Only return the conflict lanelet ids when input is a link lanelet within an intersection
            if (int_worker_->is_valid_link_lanelet_id(link_lanelet_id))
            {
                std::vector<intersection_model::lanelet_info_t> link_lanelet_v = int_worker_->get_conflict_lanelets(link_lanelet_id);
                for (std::vector<intersection_model::lanelet_info_t>::iterator itr = link_lanelet_v.begin(); itr != link_lanelet_v.end(); itr++)
                {
                    OpenAPI::OAILanelet_info lanelet_info;
                    lanelet_info.setId(itr->id);
                    lanelet_info.setSpeedLimit(itr->speed_limit);
                    lanelet_info.setLength(itr->length);
                    res.push_back(lanelet_info);
                }
                reqObj->getConflictLaneletsResponse(res);
            }
            else
            {
                QString err_msg = "Invalid link lanelet id parameter.";
                reqObj->getConflictLaneletsError(res, QNetworkReply::NetworkError::ContentAccessDenied, err_msg);
            }
        }
    }


}