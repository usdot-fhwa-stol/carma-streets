#include "kafka_consumer_worker.h"

namespace kafka_clients
{

    kafka_consumer_worker::kafka_consumer_worker(const std::string &broker_str, const std::string &topic_str,
                                                 const std::string &group_id_str, int64_t cur_offset, int32_t partition)
        :_topics_str(topic_str), _broker_str(broker_str), _group_id_str(group_id_str), _cur_offet(cur_offset),
          _partition(partition)
    {
    }

    bool kafka_consumer_worker::init()
    {
        spdlog::info("kafka_consumer_worker init()... ");

        std::string errstr;
        RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
        if (!conf)
        {
            spdlog::critical("RDKafka create global conf failed ");
            return false;
        }

        // set bootstrap server
        if (conf->set(BOOTSTRAP_SERVER, _broker_str, errstr) != RdKafka::Conf::CONF_OK)
        {
            spdlog::critical("RDKafka conf set bootstrap server failed: {0} ", errstr.c_str());
            return false;
        }

        conf->set("rebalance_cb", &_consumer_rebalance_cb, errstr);

        if (conf->set(EVENT_CB, &_consumer_event_cb, errstr) != RdKafka::Conf::CONF_OK)
        {
            spdlog::critical("RDKafka conf set event call back failed:  {0} ", errstr.c_str());
            return false;
        }

        if (conf->set(ENABLE_PARTITION_END_OF, "true", errstr) != RdKafka::Conf::CONF_OK)
        {
            spdlog::critical("RDKafka conf set partition end of failed: {0} ", errstr.c_str());
            return false;
        }

        // set consumer group
        if (conf->set(GROUP_ID, _group_id_str, errstr) != RdKafka::Conf::CONF_OK)
        {
            spdlog::critical("RDKafka conf set group id failed:  {0} ", errstr.c_str());
            return false;
        }

        if (conf->set(MAX_PARTITION_FETCH_SIZE, STR_FETCH_NUM, errstr) != RdKafka::Conf::CONF_OK)
        {
            spdlog::critical("RDKafka cof set max.partition failed:  {0} ", errstr.c_str());
        }

        // create consumer
        _consumer = RdKafka::KafkaConsumer::create(conf, errstr);
        if (!_consumer)
        {
            spdlog::critical("failed to create consumer:  {0}", errstr.c_str());
            return false;
        }

        spdlog::info("created consumer: {0} ", _consumer->name());
        delete conf;

        // create kafka topic
        RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
        if (!tconf)
        {
            spdlog::critical("RDKafka create topic conf failed ");
            return false;
        }

        _topic = RdKafka::Topic::create(_consumer, _topics_str, tconf, errstr);
        if (!_topic)
        {
            spdlog::critical("RDKafka create topic failed:  {0}", errstr.c_str());
            return false;
        }

        delete tconf;
        printCurrConf();
        return true;
    }

    void kafka_consumer_worker::stop()
    {
        _run = false;
        /*Destroy kafka instance*/ // Wait for RdKafka to decommission.
        RdKafka::wait_destroyed(5000);
    }

    void kafka_consumer_worker::subscribe()
    {
        std::vector<std::string> _topics_str_list;
        _topics_str_list.push_back(_topics_str);
        RdKafka::ErrorCode err = _consumer->subscribe(_topics_str_list);
        if (err)
        {
            spdlog::critical(" {0} Failed to subscribe to   {1} topics: {2} ", _consumer->name(), _topics_str_list.size(), RdKafka::err2str(err).c_str());
            exit(1);
        }
        spdlog::info("{0} Successfully to subscribe to   {1} topics: {2} ", _consumer->name(), _topics_str_list.size(), RdKafka::err2str(err).c_str());
        _run = true;
    }

    const char *kafka_consumer_worker::consume(int timeout_ms)
    {
        RdKafka::Message *msg = nullptr;
        msg = _consumer->consume(timeout_ms);
        const char *msg_str = msg_consume(msg, nullptr);
        return msg_str;
    }

    bool kafka_consumer_worker::is_running() const
    {
        return _run;
    }
    void kafka_consumer_worker::printCurrConf()
    {
        spdlog::info("Consumer connect to bootstrap_server: {0} , topic:  {1} , partition:  {2}, group id: {3} ",
                     (_broker_str.empty() ? "UNKNOWN" : _broker_str), (_topics_str.empty() ? "UNKNOWN" : _topics_str), _partition, (_group_id_str.empty() ? "UNKNOWN" : _group_id_str));
    }

    const char *kafka_consumer_worker::msg_consume(RdKafka::Message *message, void *opaque)
    {
        const char *return_msg_str = "";
        switch (message->err())
        {
        case RdKafka::ERR__TIMED_OUT:
            break;
        case RdKafka::ERR_NO_ERROR:
            spdlog::info(" {0} Read message at offset {1} ", _consumer->name(), message->offset());
            spdlog::info(" {0} Message Consued: {1}   bytes ):  {2}", _consumer->name(), static_cast<int>(message->len()), static_cast<const char *>(message->payload()));
            _last_offset = message->offset();
            return_msg_str = static_cast<const char *>(message->payload());
            break;
        case RdKafka::ERR__PARTITION_EOF:
            spdlog::info("{0} Reached the end of the queue, offset : {1}", _consumer->name(), _last_offset);
            break;
        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            spdlog::critical("{0} Consume failed:  {1} ", _consumer->name(), message->errstr());
            stop();
            break;

        default:
            /* Errors */
            spdlog::critical("{0} Consume failed:  {1} ", _consumer->name(), message->errstr());
            stop();
            break;
        }
        return return_msg_str;
    }
}