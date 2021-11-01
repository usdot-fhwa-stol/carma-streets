#include <thread>
#include "vehicle_status_intent_service.h"

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
    std::thread vehicle_status_intent_service_t(vehicle_status_intent_service_call);
    vehicle_status_intent_service_t.join();
    return 0;
}