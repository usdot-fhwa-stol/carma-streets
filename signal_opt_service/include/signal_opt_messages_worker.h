#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <math.h>
#include <shared_mutex>
#include <iostream>
#include <QTimer>
#include <QEventLoop>
#include <intersection_client_api_lib/OAIHelpers.h>
#include <spdlog/spdlog.h>
#include "intersection_client_api_lib/OAIDefaultApi.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"
#include "vehicle_list.h"
#include "signalized_status_intent_processor.h"
#include "spat.h"
#include "tsc_configuration_state.h"

namespace signal_opt_service
{
    class signal_opt_messages_worker: public QObject
    {

    public:
        signal_opt_messages_worker();
        ~signal_opt_messages_worker() = default;
        
        /**
         * @brief Vehicle string from kafka stream in JSON format. Calling vehicle list library to persist vehicles.
         * @param vehicle_json Vehicle string from kafka stream in JSON format.
         * @return true if the vehicle list is updated.
         * @return false if the vehilce list is not updated.
         */
        bool update_vehicle_list(const std::string& vehicle_json, std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr ) const;
        /**
         * @brief Spat string from kafka stream in JSON format. Updating the spat with the latest spat info calling the signal_phase_timing library
         * @param spat_json Spat string from kafka stream in JSON format.
         * @return true if the Spat object is updated.
         * @return false if the Spat object is not updated.
         */
        bool update_spat(const std::string& spat_json , std::shared_ptr<signal_phase_and_timing::spat> spat_ptr);

        bool update_tsc_config(const std::string &tsc_configuration, 
                                std::shared_ptr<streets_tsc_configuration::tsc_configuration_state> tsc_configuration_ptr);
        /**
         * @brief Send http GET request to intersection model at rate of configured HZ until it gets the valid (!= 0) signal group id from the intersection info.
         * Updating the intersection info with the http response that has the valid signal group id, and stop sending any more GET request.
         * @param int_info Http response from intersection model
         * @return true if intersection information is updated upon receiving valid signal group id.
         * @return false if intersection information is not updated.
         */
        bool request_intersection_info(std::shared_ptr<OpenAPI::OAIIntersection_info> _intersection_info);
    };
}