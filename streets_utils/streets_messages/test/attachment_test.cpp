#include <gtest/gtest.h>
#include <sensor_data_sharing_msg/vru/attachment.hpp>

using namespace streets_utils::messages;

TEST(attachment_test, test_from_int){
    EXPECT_EQ(attachment::UNAVAILABLE, attachment_from_int(0));
    EXPECT_EQ(attachment::STROLLER, attachment_from_int(1));
    EXPECT_EQ(attachment::BICYLE_TRAILER, attachment_from_int(2));
    EXPECT_EQ(attachment::CART, attachment_from_int(3));
    EXPECT_EQ(attachment::WHEEL_CHAIR, attachment_from_int(4));
    EXPECT_EQ(attachment::OTHER_WALK_ASSIST_ATTACHMENTS, attachment_from_int(5));
    EXPECT_EQ(attachment::PET, attachment_from_int(6));

    // Value outside of range set to unavailable
    EXPECT_EQ(attachment::UNAVAILABLE, attachment_from_int(7));

}