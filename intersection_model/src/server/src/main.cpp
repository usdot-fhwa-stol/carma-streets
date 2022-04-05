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
#include "OAIApiRouter.h"
#include "streets_configuration.h"

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

int main(int argc, char * argv[])
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


    QSharedPointer<OpenAPI::OAIApiRequestHandler> handler(new OpenAPI::OAIApiRequestHandler());
    auto router = QSharedPointer<OpenAPI::OAIApiRouter>::create(
        streets_service::streets_configuration::get_string_config("intersection_name"),
        streets_service::streets_configuration::get_int_config("intersection_id"),
        streets_service::streets_configuration::get_string_config("osm_file_path")
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

    return a.exec();
}
