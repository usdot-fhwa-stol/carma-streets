/**
 * Intersection Model API
 * No description provided (generated by Openapi Generator https://github.com/openapitools/openapi-generator)
 *
 * The version of the OpenAPI document: 1.0.0
 *
 * NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
 * https://openapi-generator.tech
 * Do not edit the class manually.
 */

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QHostAddress>
#include <QRegExp>
#include <QStringList>
#include <QSharedPointer>
#include <QObject>
#ifdef __linux__
#include <signal.h>
#include <unistd.h>
#endif
#include <qhttpengine/server.h>
#include <spdlog/spdlog.h>
#include "IntersectionModelRouter.h"
#include "streets_configuration.h"
#include "intersection_model.h"
#include "map_msg_worker.h"

#ifdef __linux__
void catchUnixSignals(QList<int> quitSignals) {
    auto handler = [](int sig) -> void {
        // blocking and not async-signal-safe func are valid
        SPDLOG_WARN("Quit the application by signal {0}", sig);
        QCoreApplication::quit();
    };

    sigset_t blocking_mask;
    sigemptyset(&blocking_mask);
    for (auto sig : quitSignals)
        sigaddset(&blocking_mask, sig);

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_mask    = blocking_mask;
    sa.sa_flags   = 0;

    for (auto sig : quitSignals)
        sigaction(sig, &sa, nullptr);
}
#endif

void intersection_model_event_update(std::shared_ptr<intersection_model::intersection_model> model, 
                                    std::shared_ptr<intersection_model::map_msg_worker> map_msp_worker, 
                                    const double lane_2_lanelet_corr_thres)
{
    auto _map_msg_consumer = map_msp_worker->get_map_msg_consumer_ptr();
    if (map_msp_worker && _map_msg_consumer)
    {
        SPDLOG_INFO("_map_msg_consumer running {0}", _map_msg_consumer->is_running());
        while (_map_msg_consumer && _map_msg_consumer->is_running())
        {
            const std::string payload = _map_msg_consumer->consume(1000);
            if (payload.length() != 0)
            {
                SPDLOG_DEBUG("Map msg payload => {0}", payload);
                bool is_updated = map_msp_worker->update_map_msg(payload);
                if(is_updated)
                {
                    auto map_msg_ptr = map_msp_worker->get_map_msg_ptr(); 
                    model->update_intersecion_info_by_map_msg(map_msg_ptr, lane_to_lanelet_corr_thres);
                }
            }
        }
    } 
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
#ifdef __linux__
    QList<int> sigs({SIGQUIT, SIGINT, SIGTERM, SIGHUP});
    catchUnixSignals(sigs);
#endif
    streets_service::streets_configuration::initialize_logger();

    // Obtain the values
    QHostAddress address = QHostAddress(QString::fromStdString(
        streets_service::streets_configuration::get_string_config("host_address")
    ));
    quint16 port = static_cast<quint16>(
        streets_service::streets_configuration::get_int_config("host_port")
    );
    
    std::shared_ptr<intersection_model::intersection_model> model ( new intersection_model::intersection_model(
        streets_service::streets_configuration::get_string_config("intersection_name"),
        streets_service::streets_configuration::get_int_config("intersection_id"),
        streets_service::streets_configuration::get_string_config("osm_file_path")
    )) ;
    QSharedPointer<OpenAPI::OAIApiRequestHandler> handler(new OpenAPI::OAIApiRequestHandler());
    auto router = QSharedPointer<IntersectionModelAPI::IntersectionModelRouter>::create(
        model
    );
    router->setUpRoutes();
    QObject::connect(handler.data(), &OpenAPI::OAIApiRequestHandler::requestReceived, [&](QHttpEngine::Socket *socket) {
        router->processRequest(socket);
    });

    QHttpEngine::Server server(handler.data());
    SPDLOG_INFO( "Serving on {0}:{1}",address.toString().toStdString(), static_cast<int>(port));
    // Attempt to listen on the specified port
    if (!server.listen(address, port)) {
        SPDLOG_CRITICAL("Unable to listen on the specified port.");
        return 1;
    }
    auto map_msp_worker = std::make_shared<intersection_model::map_msg_worker>(streets_service::streets_configuration::get_int_config("intersection_id"),
                                                                                streets_service::streets_configuration::get_string_config("bootstrap_server"),
                                                                                streets_service::streets_configuration::get_string_config("map_msg_group_id"),
                                                                                streets_service::streets_configuration::get_string_config("map_msg_topic"));
    // Get Lane to lanelet correlation threshold.The lane (MAP) to lanelet (lanelet2 map) correlation threshold in meters. 
    // If the average distance between nodes in a lane of the MAP and the centerline of the any existing intersection lanelet 
    // described in the lanelet2 map exceeds lanewidth plus this threshold they will not correlate.
    double lane_2_lanelet_corr_thres = streets_service::streets_configuration::get_double_config("lane_to_lanelet_corr_thres");
    std::thread intersection_model_events_thread(intersection_model_event_update, std::ref(model), std::ref(map_msp_worker), lane_2_lanelet_corr_thres);
    intersection_model_events_thread.detach();

    return a.exec();
}
