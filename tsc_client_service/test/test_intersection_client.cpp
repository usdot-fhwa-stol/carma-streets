#include "gtest/gtest.h"
#include "intersection_client.h"

using namespace traffic_signal_controller_service;

TEST(intersection_client, test_error_response ) {
    intersection_client client;
    ASSERT_FALSE(client.request_intersection_info());
}
