#include "gtest/gtest.h"
#include "kafka_client.h"

TEST(test_kafka_client, read_json_file_get_value_by_doc)
{
    std::string expect_val = "localhost:9092";
    auto client = std::make_shared<kafka_clients::kafka_client>();
    std::string input_bootstrap_server_param1 = "BOOTSTRAP_SERVER";
    // std::string file_name = "../test/manifest_test.json";
    // rapidjson::Document doc = client->read_json_file(file_name);
    // EXPECT_EQ(expect_val, client->get_value_by_doc(doc, input_bootstrap_server_param1.c_str()));

    // expect_val = "test";
    // std::string input_topic_param1 = "PRODUCER_TOPIC";
    // EXPECT_EQ(expect_val, client->get_value_by_doc(doc, input_topic_param1.c_str()));

    // expect_val = "test";
    // input_topic_param1 = "CONSUMER_TOPIC";
    // EXPECT_EQ(expect_val, client->get_value_by_doc(doc, input_topic_param1.c_str()));

    // expect_val = "group_one";
    // std::string input_group_id_param1 = "GROUP_ID";
    // EXPECT_EQ(expect_val, client->get_value_by_doc(doc, input_group_id_param1.c_str()));
}
