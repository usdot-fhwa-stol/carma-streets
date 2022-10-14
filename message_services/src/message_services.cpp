#include <thread>
#include "vehicle_status_intent_service.h"
#include "streets_configuration.h"


void vehicle_status_intent_service_call(std::shared_ptr<message_services::message_translations::message_lanelet2_translation> msg_translate_ptr)
{
    message_services::services::vehicle_status_intent_service s_v;
    if (s_v.initialize(msg_translate_ptr))
    {
        s_v.start();
    }
}

int main(int argc, const char **argv)
{
    streets_service::streets_configuration::initialize_logger();
    const std::string OSM_FILE_PATH = streets_service::streets_configuration::get_string_config("osm_file_path");
;

    //initialize lanelet2 message translation object
    auto msg_translate_ptr = std::make_shared<message_services::message_translations::message_lanelet2_translation>(OSM_FILE_PATH);

    std::thread vehicle_status_intent_service_t(vehicle_status_intent_service_call, std::ref(msg_translate_ptr));
    vehicle_status_intent_service_t.join();
    return 0;
}