#pragma once

#include <rapidjson/rapidjson.h>
#include <spdlog/spdlog.h>

#include "vehicle.h"
#include "all_stop_status_intent_processor.h"

namespace streets_vehicles
{
    class signalized_status_intent_processor : public all_stop_status_intent_processor
    {
    public:
        signalized_status_intent_processor() : all_stop_status_intent_processor()
        {
            SPDLOG_DEBUG("Signalized status and intent processor initialization.");
        };
        ~signalized_status_intent_processor() override = default;

    protected:
        /**
         * @brief Updates vehicle state based on previous state and stopping condition. Override the all_stop_processor vehicle update.
         * @param vehicle reference to update
         */
        void update_vehicle_state(vehicle &vehicle) const;
    };
}
