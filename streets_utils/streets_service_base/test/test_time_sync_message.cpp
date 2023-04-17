#include "time_sync_message.h"
#include <gtest/gtest.h> 

namespace streets_service::simulation {
    TEST(test_time_sync_message, json_serialization) {
        time_sync_message msg;
        msg.seq = 123;
        msg.timestep = 321;

        std::string json_message = msg.toJson();

        ASSERT_FALSE( json_message.empty() );

        time_sync_message msg_deserialize;

        msg_deserialize.fromJson(json_message);

        ASSERT_EQ(msg.seq , msg_deserialize.seq);
        ASSERT_EQ(msg.timestep, msg_deserialize.timestep);
        ASSERT_NE(msg_deserialize.seq ,0);
        ASSERT_NE(msg_deserialize.timestep ,0 );

    };
}