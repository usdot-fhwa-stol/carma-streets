#pragma once 

#include <QTimer>
#include <QEventLoop>
#include <spdlog/spdlog.h>

#include "intersection_client_api_lib/OAIDefaultApi.h"
#include "intersection_client_api_lib/OAIIntersection_info.h"

namespace traffic_signal_controller_service {
    class intersection_client : public QObject {
        private:
            std::string intersection_name;
            int intersection_id;
        public: 
            bool request_intersection_info();
            std::string get_intersection_name();
            int get_intersection_id();
    };
}