#pragma once
#include <memory>
#include "intersection_server_api_lib/OAIDefaultApiHandler.h"
#include "intersection_server_api_lib/OAIDefaultApiRequest.h"
#include "intersection_model.h"


namespace IntersectionModelAPI
{
    class IntersectionModelHandler : public OpenAPI::OAIDefaultApiHandler
    {
        Q_OBJECT
    private:
        std::shared_ptr<intersection_model::intersection_model> int_worker_;

    public:
        IntersectionModelHandler() = default;
        IntersectionModelHandler(std::shared_ptr<intersection_model::intersection_model> int_worker);
        ~IntersectionModelHandler() = default;

    public slots:
        /**
         * @brief Get the Conflict Lanelets object
         * 
         * @param link_lanelet_id 
         */
        virtual void getConflictLanelets(qint32 link_lanelet_id) override;

        /**
         * @brief Get the Intersection Info object
         */
        virtual void getIntersectionInfo() override;

        /**
         * @brief Get the list of departure lanelets for the intersection
         */
        virtual void listDepartureLanelets() override;

        /**
         * @brief Get the list of entry lanelets for the intersection
         */
        virtual void listEntryLanelets() override;

        /**
         * @brief Get the list of link lanelets for the intersection
         */
        virtual void listLinkLanelets() override;
    };

}