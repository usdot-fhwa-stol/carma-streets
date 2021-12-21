#ifndef KAFKA_CONSUMER_WORKER_H
#define KAFKA_CONSUMER_WORKER_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <sys/time.h>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

#ifndef _WIN32
#include <sys/time.h>
#else
#include <windows.h> /* for GetLocalTime */
#endif

#if _AIX
#include <unistd.h>
#endif

#include <librdkafka/rdkafkacpp.h>
namespace kafka_clients
{
    static int partition_cnt = 0;
    static int eof_cnt = 0;

    class consumer_rebalance_cb : public RdKafka::RebalanceCb 
    {
        private:
            static void part_list_print (const std::vector<RdKafka::TopicPartition*>&partitions)
            {
                for (unsigned int i = 0 ; i < partitions.size() ; i++)
                  spdlog::info("Topic {0} , Partition {1}", partitions[i]->topic() , partitions[i]->partition());
            }

        public:
            void rebalance_cb (RdKafka::KafkaConsumer *consumer, RdKafka::ErrorCode err, std::vector<RdKafka::TopicPartition*> &partitions) 
            {
                spdlog::info("RebalanceCb: {0} ", RdKafka::err2str(err) );
                part_list_print(partitions);

                RdKafka::Error *error = NULL;
                RdKafka::ErrorCode ret_err = RdKafka::ERR_NO_ERROR;

                if (err == RdKafka::ERR__ASSIGN_PARTITIONS) {
                if (consumer->rebalance_protocol() == "COOPERATIVE")
                    error = consumer->incremental_assign(partitions);
                else
                    ret_err = consumer->assign(partitions);
                    partition_cnt += (int)partitions.size();
                } 
                else 
                {
                    if (consumer->rebalance_protocol() == "COOPERATIVE") 
                    {
                        error = consumer->incremental_unassign(partitions);
                        partition_cnt -= (int)partitions.size();
                    } else {
                        ret_err = consumer->unassign();
                        partition_cnt = 0;
                    }
                }
                eof_cnt = 0; /* FIXME: Won't work with COOPERATIVE */

                if (error) {
                    spdlog::critical("incremental assign failed:  {0} ",error->str() );
                    delete error;
                } else if (ret_err)
                    spdlog::critical("assign failed:  {0} ",error->str() );
            }
    };

    class consumer_event_cb : public RdKafka::EventCb 
    {
        public:
            consumer_event_cb(){};
            ~consumer_event_cb(){};
            void event_cb (RdKafka::Event &event) 
            {
                switch (event.type())
                {
                case RdKafka::Event::EVENT_ERROR:
                    if (event.fatal()) 
                    {
                        spdlog::critical("{0} : Line {1}. FATAL  {2}  {3}",__FILE__, __LINE__,RdKafka::err2str(event.err()) ,event.str());
                    }
                    spdlog::critical("{0} : Line {1}. ERROR:  {2}  {3}",__FILE__, __LINE__,  RdKafka::err2str(event.err()) ,event.str() );
                    break;

                case RdKafka::Event::EVENT_STATS:
                    spdlog::critical("{0} : Line {1}. STATS:  {2}  {3}",__FILE__, __LINE__, RdKafka::err2str(event.err()) ,event.str() );
                    break;

                case RdKafka::Event::EVENT_LOG:
                    spdlog::critical("{0} : Line {1}. LOG:  {2}  {3} {4}",__FILE__, __LINE__, event.severity(), event.fac().c_str(), event.str().c_str() );
                    break;

                case RdKafka::Event::EVENT_THROTTLE:
                    spdlog::critical("{0} : Line {1}. THROTTLED: {2} ms by {3} id ",__FILE__, __LINE__, event.throttle_time(),(int)event.broker_id() );
                    break;

                default:
                    spdlog::critical("{0} : Line {1}. EVENT: {1} ms by {2} id ",__FILE__, __LINE__, RdKafka::err2str(event.err()),event.str() );
                    break;
                }
            }
    };
        
    class kafka_consumer_worker
    {
        private:
            const std::string BOOTSTRAP_SERVER="bootstrap.servers";
            const std::string DR_CB="dr_cb";
            const std::string EVENT_CB="event_cb";
            const std::string GROUP_ID="group.id";
            const std::string MAX_PARTITION_FETCH_SIZE="max.partition.fetch.bytes";
            const std::string ENABLE_PARTITION_END_OF="enable.partition.eof";

            //maximum size for pulling message from a single partition at a time
            std::string STR_FETCH_NUM = "10240000";
            
            std::string _topics_str = "";
            std::string _broker_str = "";
            std::string _group_id_str = "";
            int64_t _last_offset = 0;
            RdKafka::KafkaConsumer *_consumer = nullptr;
            RdKafka::Topic *_topic = nullptr;
            int64_t _cur_offet =  RdKafka::Topic::OFFSET_BEGINNING;
            int32_t _partition = 0;
            bool _run = false;
            consumer_event_cb _consumer_event_cb;
            consumer_rebalance_cb _consumer_rebalance_cb;
            const char* msg_consume(RdKafka::Message *message, void *opaque);

        public:
            kafka_consumer_worker(const std::string &broker_str, const std::string &topic_str, const std::string & group_id, int64_t cur_offset = 0, int32_t partition = 0);
            bool init();
            const char* consume(int timeout_ms);
            void subscribe();
            void stop();
            void printCurrConf();
            bool is_running();
    };
}

#endif