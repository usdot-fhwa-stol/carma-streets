#include <gtest/gtest.h>
#include "spat_projection_mode.h"

using namespace traffic_signal_controller_service;

TEST(test_spat_projection_mode, test_from_int ) {
    ASSERT_EQ( SPAT_PROJECTION_MODE::NO_PROJECTION, spat_projection_mode_from_int(0));
    ASSERT_EQ( SPAT_PROJECTION_MODE::DPP_PROJECTION, spat_projection_mode_from_int(1));
    ASSERT_EQ( SPAT_PROJECTION_MODE::FIXED_TIMING_PROJECTION, spat_projection_mode_from_int(2));
    ASSERT_EQ( SPAT_PROJECTION_MODE::NO_PROJECTION, spat_projection_mode_from_int(-1));
    ASSERT_EQ( SPAT_PROJECTION_MODE::NO_PROJECTION, spat_projection_mode_from_int(5));
}
