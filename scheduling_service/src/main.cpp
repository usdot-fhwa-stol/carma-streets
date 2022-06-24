#define RAPIDJSON_HAS_STDSTRING 1

#include <QCoreApplication>
#include "streets_configuration.h"
#include "all_stop_scheduling_service.h"

int main(int argc,char** argv)
{
    QCoreApplication a(argc, argv);
    streets_service::streets_configuration::initialize_logger();

    scheduling_service::all_stop_scheduling_service sched_service;
    if (sched_service.initialize())
    {
        sched_service.start();
    }

    return 0;

}


