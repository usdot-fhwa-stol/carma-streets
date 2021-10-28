
#include <stdlib.h> /* abs */

#include "mobility_operation_service.h"

namespace message_services
{
    namespace services
    {
        std::mutex worker_mtx;

        mobility_operation_service::mobility_operation_service() {}

        bool mobility_operation_service::initialize()
        {
            try
            {
                kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
                std::string file_path = std::string(MANIFEST_CONFIG_FILE_PATH);
                rapidjson::Document doc = client->read_json_file(file_path);

                //consumer topics
                this->mo_topic_name = client->get_value_by_doc(doc, "MO_CONSUMER_TOPIC");
                this->mo_group_id = client->get_value_by_doc(doc, "MO_GROUP_ID");

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
            std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr = std::make_shared<message_services::workers::mobilityoperation_worker>();
            run(mo_w_ptr);
        }

        void mobility_operation_service::run(std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr)
        {
            std::thread mo_t(&mobility_operation_service::msg_consumer<workers::mobilityoperation_worker>, this, std::ref(mo_w_ptr), this->mo_topic_name, this->mo_group_id);
            std::shared_ptr<models::mobilityoperation> mo_ptr = std::make_shared<models::mobilityoperation>();
            std::thread vsi_t{[mo_w_ptr, this, mo_ptr]()
                              {
                                  while (true)
                                  {
                                      if (mo_w_ptr->get_curr_list().size() > 0)
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
                                          }
                                      }

                                      std::cout << "Get Strategy Params: " << mo_ptr->getStrategy_params() << std::endl;

                                      sleep(0.1);
                                  }
                              }};

            mo_t.join();
        }

    }
}