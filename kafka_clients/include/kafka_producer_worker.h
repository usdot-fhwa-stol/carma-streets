#ifndef KAFKA_PRODUCER_WORKER_H
#define KAFKA_PRODUCER_WORKER_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#if _AIX
#include <unistd.h>
#endif

#include <librdkafka/rdkafkacpp.h>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"

namespace kafka_clients
{  
    class producer_delivery_report_cb : public RdKafka::DeliveryReportCb
    {
        public:
            producer_delivery_report_cb(){

            };
            ~producer_delivery_report_cb(){

            };
            void dr_cb (RdKafka::Message &message)
            {
                spdlog::info("Message dellivery for:  {0} bytes [ {1} ]",message.len(), message.errstr().c_str());
                if(message.key())                 
                    spdlog::info(" Key:  {:>8}",(char*)(message.key()));
            }
    };
    class producer_event_cb:public RdKafka::EventCb
    {
        public:
            producer_event_cb(){

            };
            ~producer_event_cb(){
                
            };
            void event_cb (RdKafka::Event &event) 
            {
                switch (event.type())
                {
                case RdKafka::Event::EVENT_ERROR:                 
                    spdlog::critical("{0} : Line {1}.  ERROR:  {2}  {3}",__FILE__, __LINE__, RdKafka::err2str(event.err()) ,event.str() );
                    if (event.err() == RdKafka::ERR__ALL_BROKERS_DOWN)
                    break;
                case RdKafka::Event::EVENT_STATS:
                    spdlog::critical("{0} : Line {1}.  STATS: {2}  {3}",__FILE__, __LINE__, RdKafka::err2str(event.err()) ,event.str() );
                    break;
                case RdKafka::Event::EVENT_LOG:
                    spdlog::critical("{0} : Line {1}.  LOG:  {2}  {3}",__FILE__, __LINE__, RdKafka::err2str(event.err()) ,event.str() );
                    break;
                default:
                    spdlog::critical("{0} : Line {1}.  EVENT:  {2}  {3}",__FILE__, __LINE__, RdKafka::err2str(event.err()) ,event.str() );
                    break;
                }
            }
    };

    class kafka_producer_worker
    {
        private:
            const std::string BOOTSTRAP_SERVER="bootstrap.servers";
            const std::string DR_CB="dr_cb";
            const std::string EVENT_CB="event_cb";

            RdKafka::Producer *_producer;
            RdKafka::Topic *_topic;
            std::string _topics_str;
            std::string _broker_str;
            bool _run = false;
            int _partition = 0;
            producer_delivery_report_cb _producer_delivery_report_cb;
            producer_event_cb _producer_event_cb;

        public:
            kafka_producer_worker();
            kafka_producer_worker(const std::string &brokers, const std::string &topics, int n_partition = 0);
            virtual ~kafka_producer_worker();
            bool init();
            void send(const std::string &msg);
            void stop();
            void printCurrConf();
        };
}

#endif