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

    TEST(test_time_sync_message, invalid_messages) {
        time_sync_message msg_deserialize;
        std::string not_json = "NOT JSON";

        std::string missing_seq= "{"
                                    "\"timestep\":1400"
                                 "}";

        std::string missing_timestep= "{"
                                    "\"seq\":123"
                                 "}";
        ASSERT_THROW( msg_deserialize.fromJson(not_json), std::runtime_error );
        ASSERT_THROW( msg_deserialize.fromJson(missing_seq), std::runtime_error );                         
        ASSERT_THROW( msg_deserialize.fromJson(missing_timestep), std::runtime_error );                         

    };
}