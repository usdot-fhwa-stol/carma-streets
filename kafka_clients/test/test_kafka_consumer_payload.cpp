#include "gtest/gtest.h"
#include "kafka_client.h"

#include <librdkafka/rdkafka.h>
#include <librdkafka/rdkafka_mock.h>

#include <memory>
#include <string>

/**
 * @brief Regression test for consuming Kafka payloads that are not null terminated.
 *
 * librdkafka payloads carry an explicit length and are owned by the message, so
 * kafka_consumer_worker::consume() has to size the returned string with
 * message->len(). A payload containing an embedded null proves the length is
 * honoured rather than scanned for: read as a C string it stops at the null and
 * the payload comes back truncated.
 *
 * Uses librdkafka's in-process mock cluster, so no broker is required and only
 * the public API of kafka_consumer_worker is exercised.
 */
class kafka_consumer_payload_test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        std::string err;
        RdKafka::Conf * conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
        conf->set("client.id", "mock_cluster_owner", err);
        cluster_owner.reset(RdKafka::Producer::create(conf, err));
        delete conf;
        ASSERT_NE(cluster_owner, nullptr) << err;

        mock_cluster = rd_kafka_mock_cluster_new(cluster_owner->c_ptr(), 1);
        ASSERT_NE(mock_cluster, nullptr);
        bootstraps = rd_kafka_mock_cluster_bootstraps(mock_cluster);

        RdKafka::Conf * producer_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
        producer_conf->set("bootstrap.servers", bootstraps, err);
        producer.reset(RdKafka::Producer::create(producer_conf, err));
        delete producer_conf;
        ASSERT_NE(producer, nullptr) << err;
    }

    void TearDown() override
    {
        producer.reset();
        if (mock_cluster) {
            rd_kafka_mock_cluster_destroy(mock_cluster);
        }
        cluster_owner.reset();
    }

    /// Produce the bytes exactly as given: no terminator added, embedded nulls kept.
    void produce_raw(const std::string & topic, const std::string & payload)
    {
        producer->produce(
            topic, 0, RdKafka::Producer::RK_MSG_COPY,
            const_cast<char *>(payload.data()), payload.size(),
            nullptr, 0, 0, nullptr);
        producer->flush(1000);
    }

    std::unique_ptr<RdKafka::Producer> cluster_owner;
    std::unique_ptr<RdKafka::Producer> producer;
    rd_kafka_mock_cluster_t * mock_cluster = nullptr;
    std::string bootstraps;
};

TEST_F(kafka_consumer_payload_test, consume_preserves_payload_length)
{
    // 15 bytes with a null at index 7. Read as a C string this is only 7 bytes long.
    const std::string payload = std::string("{\"a\":1}") + '\0' + "{\"b\":2}";
    ASSERT_EQ(payload.size(), 15U);

    const std::string topic = "test_payload_length";
    auto client = std::make_shared<kafka_clients::kafka_client>();
    auto consumer = client->create_consumer(bootstraps, topic, "test_payload_group");
    ASSERT_NE(consumer, nullptr);
    ASSERT_TRUE(consumer->init());
    consumer->subscribe();

    // The consumer does not set auto.offset.reset, so it starts at the latest offset.
    // Keep producing while polling until it has joined the group and been assigned the
    // partition, otherwise a single up front produce is missed.
    std::string consumed;
    for (int attempt = 0; attempt < 40 && consumed.empty(); ++attempt) {
        produce_raw(topic, payload);
        consumed = consumer->consume(500);
    }
    consumer->stop();

    ASSERT_FALSE(consumed.empty()) << "no message consumed from the mock cluster";
    // Before the fix this returned only the 7 bytes preceding the null.
    EXPECT_EQ(consumed.size(), payload.size());
    EXPECT_EQ(consumed, payload);
}
