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
#include "all_stop_status_intent_processor.h"

namespace signal_opt_service
{
    class signal_opt_messages_worker: public QObject
    {
    private:
        std::shared_ptr<OpenAPI::OAIIntersection_info> intersection_info_ptr;
        // ToDo: streets SPAT
        std::shared_ptr<streets_vehicles::vehicle_list> vehicle_list_ptr;

    public:
        signal_opt_messages_worker();
        ~signal_opt_messages_worker() = default;
        /**
         * @brief Initialize the instance variables: intersection info pointer, vehicle list pointer, and internal streets spat pointer
         */
        void initialize();
        /**
         * @brief Vehicle string from kafka stream in JSON format. Convert the vehicle JSON into streets internal vehicle object, and add the vehicle object
         * to the vehicle list. If the vehicle uniquely identified by the vehilce ID exist in the vehicle list and timestamp receiving this vehicle is larger,
         * it will update the vehicle list with latest vehicle object.
         * @param vehicle_json Vehicle string from kafka stream in JSON format.
         * @return true if the vehicle list is updated.
         * @return false if the vehilce list is not updated.
         */
        bool add_update_vehicle(const std::string& vehicle_json);
        /**
         * @brief Spat string from kafka stream in JSON format. Convert the spat JSON into streets internal spat object, and update the spat with the latest spat info
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
         * @return ** const pointer to the OpenAPI::OAIIntersection_info
         */
        const std::shared_ptr<OpenAPI::OAIIntersection_info> get_intersection_info() const;
        /**
         * @brief Get the const vehicle list pointer which does not allow caller to update the vehicle list
         * @return ** const pointer to the streets_vehicles::vehicle_list
         */
        const std::shared_ptr<streets_vehicles::vehicle_list> get_vehicle_list() const;
        
        // ToDo get spat
    };
}