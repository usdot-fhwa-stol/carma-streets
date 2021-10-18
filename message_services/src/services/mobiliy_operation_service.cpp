
#include <stdlib.h> /* abs */

#include "mobility_operation_service.h"

namespace message_services
{
    namespace services
    {
        bool mobility_operation_service::initialize()
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

        mobility_operation_service::~mobility_operation_service() {}

        void mobility_operation_service::start()
        {
            std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr = std::make_shared<message_services::workers::bsm_worker>();
            std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr = std::make_shared<message_services::workers::mobilitypath_worker>();
            std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr = std::make_shared<message_services::workers::mobilityoperation_worker>();

            run(bsm_w_ptr, mp_w_ptr, mo_w_ptr);
        }

        void mobility_operation_service::run(std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr)
        {
            std::thread mp_t(&mobility_operation_service::msg_consumer<workers::mobilitypath_worker>, this, std::ref(mp_w_ptr), this->mp_topic_name, this->mp_group_id);
            std::thread bsm_t(&mobility_operation_service::msg_consumer<workers::bsm_worker>, this, std::ref(bsm_w_ptr), this->bsm_topic_name, this->bsm_group_id);
            std::thread mo_t(&mobility_operation_service::msg_consumer<workers::mobilityoperation_worker>, this, std::ref(mo_w_ptr), this->mo_topic_name, this->mo_group_id);

            std::shared_ptr<models::bsm> bsm_ptr = std::make_shared<models::bsm>();
            std::shared_ptr<models::mobilityoperation> mo_ptr = std::make_shared<models::mobilityoperation>();
            std::shared_ptr<models::mobilitypath> mp_ptr = std::make_shared<models::mobilitypath>();

            std::thread vsi_t{[bsm_w_ptr, mo_w_ptr, mp_w_ptr, this, bsm_ptr, mo_ptr, mp_ptr]()
                              {
                                  while (true)
                                  {
                                      if (mo_w_ptr->get_curr_list().size() > 0 && bsm_w_ptr->get_curr_list().size() > 0 && mp_w_ptr->get_curr_list().size() > 0)
                                      {
                                          //Iterate mobililityoperation list with vehicle ids for all vehicles
                                          std::deque<models::mobilityoperation>::iterator itr;
                                          for (itr = mo_w_ptr->get_curr_list().begin(); itr != mo_w_ptr->get_curr_list().end(); itr++)
                                          {
                                              std::unique_lock<std::mutex> lck(worker_mtx);
                                              mo_ptr->setHeader((*itr).getHeader());
                                              mo_ptr->setStrategy((*itr).getStrategy());
                                              mo_ptr->setStrategy_params((*itr).getStrategy_params());
                                              mo_w_ptr->pop_cur_element_from_list(0); //The deque size shrik every time we call a pop element

                                              identify_latest_mapping_bsm_mp_by_mo(bsm_w_ptr, mp_w_ptr, bsm_ptr, mo_ptr, mp_ptr);
                                          }
                                      }
                                      sleep(0.1);
                                  }
                              }};

            mp_t.join();
            mo_t.join();
            bsm_t.join();
        }

        void mobility_operation_service::identify_latest_mapping_bsm_mp_by_mo(std::shared_ptr<workers::bsm_worker> bsm_w_ptr,
                                                                                 std::shared_ptr<workers::mobilitypath_worker> mp_w_ptr,
                                                                                 std::shared_ptr<models::bsm> bsm_ptr,
                                                                                 std::shared_ptr<models::mobilityoperation> mo_ptr,
                                                                                 std::shared_ptr<models::mobilitypath> mp_ptr)
        {
            //Checking timestamp for this vehicle id to find mobilitypath
            long mp_pos = 0;
            while (mp_pos < mp_w_ptr->get_curr_list().size())
            {
                std::string mp_vehicle_id = mp_w_ptr->get_curr_list().at(mp_pos).getHeader().sender_id;
                uint64_t mp_timestamp = mp_w_ptr->get_curr_list().at(mp_pos).getHeader().timestamp;
                if (mp_ptr->getHeader().sender_id == mp_vehicle_id && std::abs((long)mp_ptr->getHeader().timestamp - (long)mp_timestamp) < 100)
                {
                    mp_ptr->setHeader(mp_w_ptr->get_curr_list().at(mp_pos).getHeader());
                    mp_ptr->setTrajectory(mp_w_ptr->get_curr_list().at(mp_pos).getTrajectory());

                    mp_w_ptr->pop_cur_element_from_list(mp_pos); //The deque size shrik every time we call a pop element
                    continue;
                }
                mp_pos++;
            }

            //checking timestamp for this bsm_id
            long bsm_pos = 0;
            while (bsm_pos < bsm_w_ptr->get_curr_list().size())
            {
                if (mp_ptr->getHeader().sender_id == bsm_w_ptr->get_curr_list().at(bsm_pos).getCore_data().temprary_id && std::abs((long)mp_ptr->getHeader().timestamp - (long)bsm_w_ptr->get_curr_list().at(bsm_pos).getHeader().timestamp) < 100)
                {
                    bsm_ptr->setHeader(bsm_w_ptr->get_curr_list().at(bsm_pos).getHeader());
                    bsm_ptr->setCore_data(bsm_w_ptr->get_curr_list().at(bsm_pos).getCore_data());
                    bsm_w_ptr->pop_cur_element_from_list(bsm_pos); //The deque size shrik every time we call a pop element
                    continue;
                }
                bsm_pos++;
            }
        }

    }
}