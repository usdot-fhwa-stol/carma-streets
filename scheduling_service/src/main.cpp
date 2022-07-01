
#include <QCoreApplication>
#include "streets_configuration.h"
#include "all_stop_scheduling_service.h"
#include "intersection_client.h"

int main(int argc,char** argv)
{
    QCoreApplication a(argc, argv);
    streets_service::streets_configuration::initialize_logger();

    auto sleep_millisecs = streets_service::streets_configuration::get_int_config("sleep_millisecs");
    auto int_client_request_attempts = streets_service::streets_configuration::get_int_config("int_client_request_attempts");

    scheduling_service::all_stop_scheduling_service sched_service;
    if (sched_service.initialize(sleep_millisecs, int_client_request_attempts))
    {
        sched_service.start();
    }



    return a.exec();

}


