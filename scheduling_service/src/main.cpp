#define RAPIDJSON_HAS_STDSTRING 1

#include <QCoreApplication>
#include "streets_configuration.h"
#include "all_stop_scheduling_service.h"
#include "intersection_client.h"

int main(int argc,char** argv)
{
    QCoreApplication a(argc, argv);
    streets_service::streets_configuration::initialize_logger();

    // HTTP request to update intersection information
    auto int_client = std::make_shared<scheduling_service::intersection_client>();
    auto sleep_millisecs = std::stoul(streets_service::streets_configuration::get_string_config("sleep_millisecs"));
    auto int_client_request_attempts = std::stoul(streets_service::streets_configuration::get_string_config("int_client_request_attempts"));

    if (int_client->update_intersection_info(sleep_millisecs, int_client_request_attempts))
    {
        auto intersection_info_ptr = int_client->get_intersection_info();

        scheduling_service::all_stop_scheduling_service sched_service;
        if (intersection_info_ptr && sched_service.initialize(*intersection_info_ptr.get()))
        {
            sched_service.start();
        }
    }
    else
    {
        return false;
    }

    return a.exec();

}


