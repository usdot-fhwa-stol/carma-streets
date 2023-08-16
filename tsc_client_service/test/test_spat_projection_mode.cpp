#include <gtest/gtest.h>
#include "spat_projection_mode.h"

using namespace traffic_signal_controller_service;

TEST(test_spat_projection_mode, test_from_int ) {
    ASSERT_EQ( spat_projection_mode::no_projection, spat_projection_mode_from_int(0));
    ASSERT_EQ( spat_projection_mode::dpp_projection, spat_projection_mode_from_int(1));
    ASSERT_EQ( spat_projection_mode::fixed_projection, spat_projection_mode_from_int(2));
    ASSERT_EQ( spat_projection_mode::no_projection, spat_projection_mode_from_int(-1));
    ASSERT_EQ( spat_projection_mode::no_projection, spat_projection_mode_from_int(5));
}
