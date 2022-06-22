#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "signal_opt_service.h"

TEST(signal_opt_service, initialize)
{
    signal_opt_service::signal_opt_service so_service;
    ASSERT_FALSE(so_service.initialize());
}

TEST(signal_opt_service, update_intersection_info)
{
    signal_opt_service::signal_opt_service so_service;
    ASSERT_FALSE(so_service.initialize());
    ASSERT_FALSE(so_service.update_intersection_info(1000, 1));
}