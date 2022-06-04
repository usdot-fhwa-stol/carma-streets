#include "IntersectionModelRouter.h"

namespace IntersectionModelAPI
{
    IntersectionModelRouter::IntersectionModelRouter(std::shared_ptr<intersection_model::intersection_model> int_worker)
    {
        SPDLOG_DEBUG("IntersectionModelRouter constructor called");
        auto mIntersectionModelHandler = QSharedPointer<IntersectionModelHandler>::create(int_worker);
        if (mIntersectionModelHandler != nullptr)
        {
            setOAIDefaultApiHandler(mIntersectionModelHandler);
        }
    }
}
