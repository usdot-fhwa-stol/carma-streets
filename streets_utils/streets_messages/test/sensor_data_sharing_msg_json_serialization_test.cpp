#include <gtest/gtest.h>
#include <serializers/sensor_data_sharing_msg_json_serializer.hpp>
#include "json_utils.hpp"

using namespace streets_utils::messages;
using namespace streets_utils::json_utils;

TEST(sensor_data_sharing_msg_json_serialization_test, test) {
    sensor_data_sharing_msg msg;
    // Serialize Msg
    std::string json = to_json(msg);
    // String is not empty
    EXPECT_FALSE(json.empty());
    // Confirm result has msg information in desired structure
    auto result = parse_json(json);
    EXPECT_TRUE(result.HasMember("sdsm_time_stamp"));
    EXPECT_TRUE(result.FindMember("sdsm_time_stamp")->value.IsObject());
}