
#include <stdlib.h> /* abs */

#include "vehicle_status_intent_service.h"

namespace message_services
{

    namespace services
    {
        std::mutex worker_mtx;
        vehicle_status_intent_service::vehicle_status_intent_service() {}

        bool vehicle_status_intent_service::initialize()
        {
            try
            {
                kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
                std::string file_path = std::string(MANIFEST_CONFIG_FILE_PATH);
                rapidjson::Document doc = client->read_json_file(file_path);

                //consumer topics
                this->bsm_topic_name = client->get_value_by_doc(doc, "BSM_CONSUMER_TOPIC");
                this->bsm_group_id = client->get_value_by_doc(doc, "BSM_GROUP_ID");

                this->mp_topic_name = client->get_value_by_doc(doc, "MP_CONSUMER_TOPIC");
                this->mp_group_id = client->get_value_by_doc(doc, "MP_GROUP_ID");

                this->mo_topic_name = client->get_value_by_doc(doc, "MO_CONSUMER_TOPIC");
                this->mo_group_id = client->get_value_by_doc(doc, "MO_GROUP_ID");

                //producer topics
                this->vsi_topic_name = client->get_value_by_doc(doc, "VSI_PRODUCER_TOPIC");

                //kafka config
                this->bootstrap_server = client->get_value_by_doc(doc, "BOOTSTRAP_SERVER");

                delete client;
                return true;
            }
            catch (std::exception ex)
            {
                spdlog::info("Vehicle status Intent Service Initialization failure: ", ex.what());
                return false;
            }
        }
        vehicle_status_intent_service::~vehicle_status_intent_service() {}

        void vehicle_status_intent_service::start()
        {
            std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr = std::make_shared<message_services::workers::bsm_worker>();
            std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr = std::make_shared<message_services::workers::mobilitypath_worker>();
            std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr = std::make_shared<message_services::workers::mobilityoperation_worker>();
            std::shared_ptr<message_services::workers::vehicle_status_intent_worker> vsi_w_ptr = std::make_shared<message_services::workers::vehicle_status_intent_worker>();
            // run(bsm_w_ptr, mp_w_ptr, mo_w_ptr);
            run(vsi_w_ptr, bsm_w_ptr, mp_w_ptr, mo_w_ptr);
        }

        void vehicle_status_intent_service::run(std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr)
        {
            std::thread mp_t(&vehicle_status_intent_service::msg_consumer<workers::mobilitypath_worker>, this, std::ref(mp_w_ptr), this->mp_topic_name, this->mp_group_id);
            std::thread bsm_t(&vehicle_status_intent_service::msg_consumer<workers::bsm_worker>, this, std::ref(bsm_w_ptr), this->bsm_topic_name, this->bsm_group_id);
            std::thread mo_t(&vehicle_status_intent_service::msg_consumer<workers::mobilityoperation_worker>, this, std::ref(mo_w_ptr), this->mo_topic_name, this->mo_group_id);

            std::shared_ptr<models::vehicle_status_intent> vsi_ptr = std::make_shared<models::vehicle_status_intent>();
            std::shared_ptr<models::bsm> bsm_ptr = std::make_shared<models::bsm>();
            std::shared_ptr<models::mobilityoperation> mo_ptr = std::make_shared<models::mobilityoperation>();
            std::shared_ptr<models::mobilitypath> mp_ptr = std::make_shared<models::mobilitypath>();

            std::thread vsi_t{[bsm_w_ptr, mo_w_ptr, mp_w_ptr, this, bsm_ptr, mo_ptr, mp_ptr, vsi_ptr]()
                              {
                                  while (true)
                                  {
                                      if (mo_w_ptr->get_curr_list().size() > 0 && bsm_w_ptr->get_curr_list().size() > 0 && mp_w_ptr->get_curr_list().size() > 0)
                                      {
                                          std::unique_lock<std::mutex> lck(worker_mtx);
                                          identify_latest_mapping_bsm_mo_mp(bsm_w_ptr, mo_w_ptr, mp_w_ptr, bsm_ptr, mo_ptr, mp_ptr);

                                          *vsi_ptr = compose_vehicle_status_intent(*bsm_ptr, *mo_ptr, *mp_ptr);

                                          std::string msg_to_pub = vsi_ptr->asJson();
                                          this->publish_msg<const char *>(msg_to_pub.c_str(), this->vsi_topic_name);
                                      }
                                      sleep(0.1);
                                  }
                              }};

            vsi_t.join();
            mp_t.join();
            mo_t.join();
            bsm_t.join();
        }

        /**
         * //a global Map key vehicle id, if (timestamp diff)_> update with bsm id to correlate messages. 
         * Only add when the vehicle id does not exist
            //Pull at 10HZ and publish leave all entries; have not yet updated (timestamp now -> bsm timestamp) in 10 secs
        ***/
        void vehicle_status_intent_service::run(std::shared_ptr<workers::vehicle_status_intent_worker> vsi_w_ptr,
                                                std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr)
        {
            //Consuming MobilityPath messages
            std::thread vsi_mp_t{[mp_w_ptr, vsi_w_ptr, this]()
                                 {
                                     kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
                                     kafka_clients::kafka_consumer_worker *consumer_worker = client->create_consumer(this->bootstrap_server, this->mp_topic_name, this->mp_group_id);
                                     delete client;

                                     if (!consumer_worker->init())
                                     {
                                         spdlog::critical("kafka consumer initialize error");
                                     }
                                     else
                                     {
                                         consumer_worker->subscribe();
                                         if (!consumer_worker->is_running())
                                         {
                                             spdlog::critical("consumer_worker is not running");
                                         }

                                         while (consumer_worker->is_running())
                                         {
                                             const char *payload = consumer_worker->consume(1000);
                                             // spdlog::info("bsm message payload: {0}", payload);
                                             if (std::strlen(payload) != 0 && mp_w_ptr)
                                             {
                                                 std::unique_lock<std::mutex> lck(worker_mtx);
                                                 mp_w_ptr->process_incoming_msg(payload);
                                                 vsi_w_ptr->update_insert_by_incoming_mobilitypath_msg(mp_w_ptr->get_curr_list().back());
                                                 mp_w_ptr->pop_cur_element_from_list(0);
                                             }

                                             if (!mp_w_ptr)
                                             {
                                                 spdlog::critical("Message worker is not initialized");
                                             }
                                         }
                                         consumer_worker->stop();
                                     }
                                     delete consumer_worker;
                                 }};

            //Consuming MobilityOperation messages
            std::thread vsi_mo_t{[mo_w_ptr, vsi_w_ptr, this]()
                                 {
                                     kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
                                     kafka_clients::kafka_consumer_worker *consumer_worker = client->create_consumer(this->bootstrap_server, this->mo_topic_name, this->mo_group_id);
                                     delete client;

                                     if (!consumer_worker->init())
                                     {
                                         spdlog::critical("kafka consumer initialize error");
                                     }
                                     else
                                     {
                                         consumer_worker->subscribe();
                                         if (!consumer_worker->is_running())
                                         {
                                             spdlog::critical("consumer_worker is not running");
                                         }

                                         while (consumer_worker->is_running())
                                         {
                                             const char *payload = consumer_worker->consume(1000);
                                             // spdlog::info("bsm message payload: {0}", payload);
                                             if (std::strlen(payload) != 0 && mo_w_ptr)
                                             {
                                                 std::unique_lock<std::mutex> lck(worker_mtx);
                                                 mo_w_ptr->process_incoming_msg(payload);
                                                 vsi_w_ptr->update_insert_by_incoming_mobilityoperation_msg(mo_w_ptr->get_curr_list().back());
                                                 mo_w_ptr->pop_cur_element_from_list(0);
                                             }

                                             if (!mo_w_ptr)
                                             {
                                                 spdlog::critical("Message worker is not initialized");
                                             }
                                         }
                                         consumer_worker->stop();
                                     }
                                     delete consumer_worker;
                                 }};

            //Consuming BSM messages
            std::thread vsi_bsm_t{[bsm_w_ptr, vsi_w_ptr, this]()
                                  {
                                      kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
                                      kafka_clients::kafka_consumer_worker *consumer_worker = client->create_consumer(this->bootstrap_server, this->bsm_topic_name, this->bsm_group_id);
                                      delete client;

                                      if (!consumer_worker->init())
                                      {
                                          spdlog::critical("kafka consumer initialize error");
                                      }
                                      else
                                      {
                                          consumer_worker->subscribe();
                                          if (!consumer_worker->is_running())
                                          {
                                              spdlog::critical("consumer_worker is not running");
                                          }

                                          while (consumer_worker->is_running())
                                          {
                                              const char *payload = consumer_worker->consume(1000);
                                              // spdlog::info("bsm message payload: {0}", payload);
                                              if (std::strlen(payload) != 0 && bsm_w_ptr)
                                              {
                                                  std::unique_lock<std::mutex> lck(worker_mtx);
                                                  bsm_w_ptr->process_incoming_msg(payload);
                                                  vsi_w_ptr->update_by_incoming_bsm_msg(bsm_w_ptr->get_curr_list().back());
                                                  bsm_w_ptr->pop_cur_element_from_list(0);
                                              }

                                              if (!bsm_w_ptr)
                                              {
                                                  spdlog::critical("Message worker is not initialized");
                                              }
                                          }
                                          consumer_worker->stop();
                                      }
                                      delete consumer_worker;
                                  }};

            //Publishing Vehicle Status and Intent messages
            std::thread vsi_t{[vsi_w_ptr, this]()
                              {
                                  while (true)
                                  {
                                      if (vsi_w_ptr->get_curr_map().size() > 0)
                                      {
                                          std::unique_lock<std::mutex> lck(worker_mtx);
                                          std::map<std::string, models::vehicle_status_intent>::iterator itr = vsi_w_ptr->get_curr_map().begin();
                                          while (itr != vsi_w_ptr->get_curr_map().end())
                                          {
                                              std::string msg_to_pub = itr->second.asJson();
                                              this->publish_msg<const char *>(msg_to_pub.c_str(), this->vsi_topic_name);
                                              spdlog::info("vsi_t msg_to_pub: {0} ", msg_to_pub);
                                              ++itr;
                                          }
                                      }
                                      sleep(0.1);
                                  }
                              }};

            vsi_t.join();
            vsi_mp_t.join();
            vsi_mo_t.join();
            vsi_bsm_t.join();
        }

        void vehicle_status_intent_service::identify_latest_mapping_bsm_mo_mp(std::shared_ptr<workers::bsm_worker> bsm_w_ptr,
                                                                              std::shared_ptr<workers::mobilityoperation_worker> mo_w_ptr,
                                                                              std::shared_ptr<workers::mobilitypath_worker> mp_w_ptr,
                                                                              std::shared_ptr<models::bsm> bsm_ptr,
                                                                              std::shared_ptr<models::mobilityoperation> mo_ptr,
                                                                              std::shared_ptr<models::mobilitypath> mp_ptr)
        {
            //Identify which vehicle by vehicle id to find mobililityoperation
            std::deque<models::mobilityoperation>::iterator itr;
            long mo_pos = 0;
            for (itr = mo_w_ptr->get_curr_list().begin(); itr != mo_w_ptr->get_curr_list().end(); itr++)
            {
                std::string strategy_params = (*itr).getStrategy_params().c_str();
                std::string mo_vehicle_id = (*itr).getHeader().sender_id;
                uint64_t mo_timestamp = (*itr).getHeader().timestamp;
                std::string mo_bsm_id = (*itr).getHeader().sender_bsm_id;
                mo_ptr->setHeader((*itr).getHeader());
                mo_ptr->setStrategy((*itr).getStrategy());
                mo_ptr->setStrategy_params((*itr).getStrategy_params());
                mo_w_ptr->pop_cur_element_from_list(mo_pos);

                //Checking timestamp for this vehicle id to find mobilitypath
                long mp_pos = 0;
                for (auto mp_v_item : mp_w_ptr->get_curr_list())
                {
                    std::string mp_vehicle_id = mp_v_item.getHeader().sender_id;
                    uint64_t mp_timestamp = mp_v_item.getHeader().timestamp;
                    if (mo_vehicle_id == mp_vehicle_id && std::abs((long)mo_timestamp - (long)mp_timestamp) < 100)
                    {
                        mp_ptr->setHeader(mp_v_item.getHeader());
                        mp_ptr->setTrajectory(mp_v_item.getTrajectory());

                        mp_w_ptr->pop_cur_element_from_list(mp_pos);
                    }
                    mp_pos++;
                }

                //checking timestamp for this bsm_id
                long bsm_pos = 0;
                for (auto bsm_v_item : bsm_w_ptr->get_curr_list())
                {
                    std::string bsm_id = bsm_v_item.getCore_data().temprary_id;
                    uint64_t bsm_timestamp = bsm_v_item.getHeader().timestamp;
                    if (mo_bsm_id == bsm_id && std::abs((long)mo_timestamp - (long)bsm_timestamp) < 100)
                    {
                        bsm_ptr->setHeader(bsm_v_item.getHeader());
                        bsm_ptr->setCore_data(bsm_v_item.getCore_data());
                        bsm_w_ptr->pop_cur_element_from_list(bsm_pos);
                    }
                    bsm_pos++;
                }
                mo_pos++;
            }
        }

        models::vehicle_status_intent vehicle_status_intent_service::compose_vehicle_status_intent(models::bsm &bsm,
                                                                                                   models::mobilityoperation &mo,
                                                                                                   models::mobilitypath &mp)
        {
            models::vehicle_status_intent vsi;
            vsi.setVehicle_id(mo.getHeader().sender_id);
            vsi.setVehicle_length(bsm.getCore_data().size.length);
            vsi.setCur_speed(bsm.getCore_data().speed);
            vsi.setCur_timestamp(bsm.getHeader().timestamp);

            //Todo: fill out other info from bsm, mobilitypath and mobilityoperation
            return vsi;
        }

        template <typename T>
        void vehicle_status_intent_service::msg_consumer(std::shared_ptr<T> msg_w_ptr, std::string topic, std::string group_id)
        {
            kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
            kafka_clients::kafka_consumer_worker *consumer_worker = client->create_consumer(this->bootstrap_server, topic, group_id);
            delete client;

            if (!consumer_worker->init())
            {
                spdlog::critical("kafka consumer initialize error");
            }
            else
            {
                consumer_worker->subscribe();
                if (!consumer_worker->is_running())
                {
                    spdlog::critical("consumer_worker is not running");
                }

                while (consumer_worker->is_running())
                {
                    const char *payload = consumer_worker->consume(1000);
                    // spdlog::info("bsm message payload: {0}", payload);
                    if (std::strlen(payload) != 0 && msg_w_ptr)
                    {
                        std::unique_lock<std::mutex> lck(worker_mtx);
                        msg_w_ptr->process_incoming_msg(payload);
                    }

                    if (!msg_w_ptr)
                    {
                        spdlog::critical("Message worker is not initialized");
                    }
                }
                consumer_worker->stop();
            }
            delete consumer_worker;
            return;
        }

        template <typename T>
        void vehicle_status_intent_service::publish_msg(T msg, std::string topic)
        {
            kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
            kafka_clients::kafka_producer_worker *producer_worker = client->create_producer(this->bootstrap_server, topic);
            delete client;

            std::string msg_to_send = "";
            msg_to_send = (char *)msg;
            if (!producer_worker->init())
            {
                spdlog::critical("kafka producer initialize error");
            }
            else
            {
                if (msg_to_send.length() > 0)
                {
                    producer_worker->send(msg_to_send);
                    delete producer_worker;
                }
            }
            return;
        }
    }
}