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

namespace signal_opt_service
{
    class signal_opt_messages_worker: public QObject
    {
    private:
        std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr;
        std::shared_ptr<signal_phase_and_timing::spat> spat_ptr;
        std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr;

    public:
        signal_opt_messages_worker();
        ~signal_opt_messages_worker() = default;
        
        /**
         * @brief Vehicle string from kafka stream in JSON format. Calling vehicle list library to persist vehicles.
         * @param vehicle_json Vehicle string from kafka stream in JSON format.
         * @return true if the vehicle list is updated.
         * @return false if the vehilce list is not updated.
         */
        bool add_update_vehicle(const std::string& vehicle_json) const;
        /**
         * @brief Spat string from kafka stream in JSON format. Updating the spat with the latest spat info calling the signal_phase_timing library
         * @param spat_json Spat string from kafka stream in JSON format.
         * @return true if the Spat object is updated.
         * @return false if the Spat object is not updated.
         */
        bool update_spat(const std::string& spat_json);
        /**
         * @brief Send http GET request to intersection model at rate of configured HZ until it gets the valid (!= 0) signal group id from the intersection info.
         * Updating the intersection info with the http response that has the valid signal group id, and stop sending any more GET request.
         * @param int_info Http response from intersection model
         * @return true if intersection information is updated upon receiving valid signal group id.
         * @return false if intersection information is not updated.
         */
        bool request_intersection_info();
        /**
         * @brief Get the const intersection info pointer which does not allow caller to update the intersection info
         * @return A constant intersection info pointer to prevent any update
         */
        const std::shared_ptr<OpenAPI::OAIIntersection_info>& get_intersection_info() const;
        /**
         * @brief Get the const vehicle list pointer which does not allow caller to update the vehicle list
         * @return A constant vehicle list pointer to prevent any update
         */
        const std::shared_ptr<streets_vehicles::vehicle_list>& get_vehicle_list() const;
        /**
         * @brief Get the latest spat object pointer
         * @return A constant spat pointer to prevent any update
         */
        const std::shared_ptr<signal_phase_and_timing::spat>& get_latest_spat() const;
    };
}