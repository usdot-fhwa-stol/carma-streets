
#include "kafka_producer_worker.h"
namespace kafka_clients
{
    kafka_producer_worker::kafka_producer_worker(const std::string &brokers, const std::string &topics, int partition)
    :_run(true),_topics_str(topics),_broker_str(brokers),_partition(partition)
    {
        // std::cout<<"\nkafka_producer_worker init()...\n";
        spdlog::info("kafka_producer_worker init()... ");
        _topic = NULL;
        _producer = NULL;
    }
    kafka_producer_worker::kafka_producer_worker()
    {
        _topic = NULL;
        _producer = NULL;
        _run = true;
    }

    kafka_producer_worker::~kafka_producer_worker()
    {
        stop();
    }

    bool kafka_producer_worker::init()
    {
        std::string errstr ="";

        //Create configuration objects
        RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
        RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

        /***
         * Set Configuration properties
        */   
        //set broker list
        if(conf->set(BOOTSTRAP_SERVER, _broker_str, errstr) != RdKafka::Conf::CONF_OK)
        {
            spdlog::critical("RdKafka conf set brokerlist failed: {0} ", errstr.c_str());
            return false;
        }

        //set delivery report callback
        if(conf->set(DR_CB,&_producer_delivery_report_cb,errstr) !=RdKafka::Conf::CONF_OK)
        {
            spdlog::critical("RdKafka conf set delivery report callback failed:   {0}", errstr.c_str());
            return false;
        }

        //set event callback
        if(conf->set(EVENT_CB, &_producer_event_cb, errstr) !=RdKafka::Conf::CONF_OK)
        {
            spdlog::critical("RdKafka conf set event callback failed: {0} ", errstr.c_str());
            return false;
        }

        // create producer using accumulated global configuration.
        _producer = RdKafka::Producer::create(conf,errstr);
        if(!_producer)
        {
            spdlog::critical("Failed to create producer:  {0} ", errstr.c_str());
            return false;
        }
        delete conf;

        spdlog::info("created producer:  {:>8} ",  _producer->name());

        // Create topic handle
        _topic = RdKafka::Topic::create(_producer, _topics_str, tconf, errstr);
        if(!_topic)
        {
            spdlog::critical("Failed to create producer:  {0} ", errstr.c_str());
            
            return false;
        }
        delete tconf;
        printCurrConf();
        return true;
    }

    void kafka_producer_worker::send(const std::string &msg)
    {
        
        if(!_run)
            return;
        
        if(msg.empty()){
            _producer->poll(0);
            return;
        }
            
        //produce messages
        while(true)
        {
            RdKafka::ErrorCode resp = _producer->produce(_topic, 
                                                    _partition, 
                                                    RdKafka::Producer::RK_MSG_COPY,
                                                    const_cast<char *>(msg.c_str()),
                                                    msg.size(), 
                                                    NULL, 
                                                    NULL);
             if(resp != RdKafka::ERR_NO_ERROR)
             {
                // std::cerr << _producer->name()<< " Produce failed: " << RdKafka::err2str(resp) << std::endl;
                spdlog::critical(" {0} Produce failed:  {1} ", _producer->name(), RdKafka::err2str(resp));
                if (resp == RdKafka::ERR__QUEUE_FULL) {
                    /* If the internal queue is full, wait for
                    * messages to be delivered and then retry.
                    * The internal queue represents both
                    * messages to be sent and messages that have
                    * been sent or failed, awaiting their
                    * delivery report callback to be called.
                    *
                    * The internal queue is limited by the
                    * configuration property
                    * queue.buffering.max.messages */
                    _producer->poll(1000/*block for max 1000ms*/);
                    continue;
                 }
            }
            else{
                std::cerr << _producer->name()<< " Produced message ( " << msg.size() << " bytes )" <<" , message content: "<<msg.c_str() << std::endl;
            }

            //break the loop regardless of sucessfully sent or failed
            break;
        }


        /* A producer application should continually serve
        * the delivery report queue by calling poll()
        * at frequent intervals.
        * Either put the poll call in your main loop, or in a
        * dedicated thread, or call it after every produce() call.
        * Just make sure that poll() is still called
        * during periods where you are not producing any messages
        * to make sure previously produced messages have their
        * delivery report callback served (and any other callbacks
        * you register). */       
        _producer->poll(0);
    }

    void kafka_producer_worker::stop()
    {
         /* Wait for final messages to be delivered or fail.
        * flush() is an abstraction over poll() which
        * waits for all messages to be delivered. */
        spdlog::critical("Stopping producer client.. ");
        spdlog::critical("Flushing final messages... ");
        
       
        if (_topic) {
            delete _topic;
        }

        if (_producer) {
            _producer->flush(10*1000 /* wait for max 10 seconds */);

            if (_producer->outq_len() > 0)
                spdlog::info("  {0} {1} message(s) were not delivered  ", _producer->name(), _producer->outq_len());
            delete _producer;
        }
    }

    void kafka_producer_worker::printCurrConf()
    {
         spdlog::info("Producer connect to bootstrap_server: {0}, topic: {1} ,partition: {2} ",
        (_broker_str.empty() ?"UNKNOWN":_broker_str) ,(_topics_str.empty() ?"UNKNOWN":_topics_str),_partition);
       
    }
}