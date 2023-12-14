#include <gtest/gtest.h>
#include "vehicle_status_intent_service.h"

TEST(test_vehicle_status_intent_service, initialize)
{
    message_services::services::vehicle_status_intent_service service;
    streets_service::streets_configuration::create("/home/carma-streets/message_services/manifest.json");

    ASSERT_FALSE(service.initialize());
}