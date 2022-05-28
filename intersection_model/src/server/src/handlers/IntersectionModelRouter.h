#pragma once
#include "spdlog/spdlog.h"
#include "intersection_server_api_lib/OAIApiRouter.h"
#include "IntersectionModelHandler.h"

namespace IntersectionModelAPI
{
    class IntersectionModelRouter : public OpenAPI::OAIApiRouter
    {
    public:
        IntersectionModelRouter() = default;
        IntersectionModelRouter(std::shared_ptr<intersection_model::intersection_model> int_worker);
        ~IntersectionModelRouter() = default;
    };
}