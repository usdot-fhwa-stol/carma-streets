#include <gtest/gtest.h>
#include "vehicle_status_intent_service.h"

TEST(test_vehicle_status_intent_service, initialize)
{
    message_services::services::vehicle_status_intent_service service;
    ASSERT_FALSE(service.initialize());
}