#include <thread>
#include "vehicle_status_intent_service.h"
#include "streets_configuration.h"


void vehicle_status_intent_service_call()
{
    message_services::services::vehicle_status_intent_service s_v;
    if (s_v.initialize())
    {
        s_v.start();
    }
}

int main(int argc, const char **argv)
{
    // TODO: Replace initialization by extending streets_service_base and overiding initialize and start methods
    streets_service::streets_configuration::create("../manifest.json");
    streets_service::streets_configuration::initialize_logger();

    std::thread vehicle_status_intent_service_t(vehicle_status_intent_service_call);
    vehicle_status_intent_service_t.join();
    return 0;
}