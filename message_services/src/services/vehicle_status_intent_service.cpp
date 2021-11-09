
#include <stdlib.h> /* abs */

#include "vehicle_status_intent_service.h"

namespace message_services
{

    namespace services
    {
        std::mutex worker_mtx;
        vehicle_status_intent_service::vehicle_status_intent_service() {}

        bool vehicle_status_intent_service::initialize(std::shared_ptr<message_translations::message_lanelet2_translation> msg_lanelet2_translate_ptr)
        {
            try
            {
                kafka_clients::kafka_client *client = new kafka_clients::kafka_client();
                std::string file_path = std::string(MANIFEST_CONFIG_FILE_PATH);
                rapidjson::Document doc = client->read_json_file(file_path);

                //kafka config
                this->bootstrap_server = client->get_value_by_doc(doc, "BOOTSTRAP_SERVER");

                //consumer topics
                this->bsm_topic_name = client->get_value_by_doc(doc, "BSM_CONSUMER_TOPIC");
                this->bsm_group_id = client->get_value_by_doc(doc, "BSM_GROUP_ID");

                this->mp_topic_name = client->get_value_by_doc(doc, "MP_CONSUMER_TOPIC");
                this->mp_group_id = client->get_value_by_doc(doc, "MP_GROUP_ID");

                this->mo_topic_name = client->get_value_by_doc(doc, "MO_CONSUMER_TOPIC");
                this->mo_group_id = client->get_value_by_doc(doc, "MO_GROUP_ID");

                //producer topics
                this->vsi_topic_name = client->get_value_by_doc(doc, "VSI_PRODUCER_TOPIC");

                _bsm_consumer_worker = client->create_consumer(this->bootstrap_server, this->bsm_topic_name, this->bsm_group_id);
                _mp_consumer_worker = client->create_consumer(this->bootstrap_server, this->mp_topic_name, this->mp_group_id);
                _mo_consumer_worker = client->create_consumer(this->bootstrap_server, this->mo_topic_name, this->mo_group_id);

                if (!_bsm_consumer_worker->init() || !_mp_consumer_worker->init() || !_mo_consumer_worker->init())
                {
                    spdlog::critical("kafka consumers (_bsm_consumer_worker, _mp_consumer_worker or _mo_consumer_worker) initialize error");
                }
                else
                {
                    _bsm_consumer_worker->subscribe();
                    _mp_consumer_worker->subscribe();
                    _mo_consumer_worker->subscribe();
                    if (!_bsm_consumer_worker->is_running() || !_mp_consumer_worker->is_running() || !_mo_consumer_worker->is_running())
                    {
                        spdlog::critical("consumer_workers (_bsm_consumer_worker, _mp_consumer_worker or _mo_consumer_worker) is not running");
                        exit(-1);
                    }
                }

                this->_vsi_producer_worker = client->create_producer(this->bootstrap_server, this->vsi_topic_name);
                if (!_vsi_producer_worker->init())
                {
                    spdlog::critical("kafka producer (_vsi_producer_worker) initialize error");
                    exit(-1);
                }

                this->vsi_est_path_point_count = std::stoi(client->get_value_by_doc(doc, "VSI_EST_PATH_COUNT"));

                delete client;

                this->_msg_lanelet2_translate_ptr = msg_lanelet2_translate_ptr;

                return true;
            }
            catch (std::exception ex)
            {
                spdlog::info("Vehicle status Intent Service Initialization failure: ", ex.what());
                return false;
            }
        }

        vehicle_status_intent_service::~vehicle_status_intent_service()
        {
            if (_bsm_consumer_worker)
            {
                _bsm_consumer_worker->stop();
                delete _bsm_consumer_worker;
            }

            if (_mo_consumer_worker)
            {
                _mo_consumer_worker->stop();
                delete _mo_consumer_worker;
            }

            if (_mp_consumer_worker)
            {
                _mp_consumer_worker->stop();
                delete _mp_consumer_worker;
            }
        }

        void vehicle_status_intent_service::start()
        {
            std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr = std::make_shared<message_services::workers::bsm_worker>();
            std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr = std::make_shared<message_services::workers::mobilitypath_worker>();
            std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr = std::make_shared<message_services::workers::mobilityoperation_worker>();
            std::shared_ptr<message_services::workers::vehicle_status_intent_worker> vsi_w_ptr = std::make_shared<message_services::workers::vehicle_status_intent_worker>();
            run(bsm_w_ptr, mp_w_ptr, mo_w_ptr);
            // run(vsi_w_ptr, bsm_w_ptr, mp_w_ptr, mo_w_ptr);
        }

        void vehicle_status_intent_service::run(std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr)
        {
            std::thread mp_t(&vehicle_status_intent_service::msg_consumer<workers::mobilitypath_worker>, this, std::ref(mp_w_ptr), this->_mp_consumer_worker);
            std::thread bsm_t(&vehicle_status_intent_service::msg_consumer<workers::bsm_worker>, this, std::ref(bsm_w_ptr), this->_bsm_consumer_worker);
            std::thread mo_t(&vehicle_status_intent_service::msg_consumer<workers::mobilityoperation_worker>, this, std::ref(mo_w_ptr), this->_mo_consumer_worker);

            std::shared_ptr<models::vehicle_status_intent> vsi_ptr = std::make_shared<models::vehicle_status_intent>();
            std::shared_ptr<models::bsm> bsm_ptr = std::make_shared<models::bsm>();
            std::shared_ptr<models::mobilityoperation> mo_ptr = std::make_shared<models::mobilityoperation>();
            std::shared_ptr<models::mobilitypath> mp_ptr = std::make_shared<models::mobilitypath>();

            std::thread vsi_t{[bsm_w_ptr, mo_w_ptr, mp_w_ptr, this, bsm_ptr, mo_ptr, mp_ptr, vsi_ptr]()
                              {
                                  while (true)
                                  {
                                      //Change spdlog from debug to info for printing output in terminal
                                      spdlog::debug("Processing the BSM list size: {0}", bsm_w_ptr->get_curr_list().size());
                                      spdlog::debug("Processing the MobilityOperation list size: {0}", mo_w_ptr->get_curr_list().size());
                                      spdlog::debug("Processing the MobilityPath list size: {0}", mp_w_ptr->get_curr_list().size());
                                      if (mo_w_ptr && mo_w_ptr->get_curr_list().size() > 0 && bsm_w_ptr && bsm_w_ptr->get_curr_list().size() > 0 && mp_w_ptr && mp_w_ptr->get_curr_list().size() > 0)
                                      {
                                          spdlog::debug("Processing the BSM, mobilityOperation and MP from list...");
                                          std::unique_lock<std::mutex> lck(worker_mtx);

                                          //Iterate mobililityoperation list with vehicle ids for all vehicles
                                          for (auto itr = mo_w_ptr->get_curr_list().begin(); itr != mo_w_ptr->get_curr_list().end(); itr++)
                                          {
                                              spdlog::debug("Current mobilityOperation list SIZE = {0}", mo_w_ptr->get_curr_list().size());
                                              if (mo_w_ptr && mo_ptr && !mo_w_ptr->get_curr_list().empty())
                                              {
                                                  mo_ptr->setHeader((*itr).getHeader());
                                                  mo_ptr->setStrategy((*itr).getStrategy());
                                                  mo_ptr->setStrategy_params((*itr).getStrategy_params());

                                                  if (identify_latest_mapping_bsm_mp_by_mo(bsm_w_ptr, mp_w_ptr, bsm_ptr, mo_ptr, mp_ptr))
                                                  {
                                                      spdlog::info("Done mapping BSM, MobilityPath messages using MobilityOperation");
                                                      mo_w_ptr->pop_cur_element_from_list(0); //The deque size shrik every time we call a pop element

                                                      *vsi_ptr = compose_vehicle_status_intent(*bsm_ptr, *mo_ptr, *mp_ptr);
                                                      if (vsi_ptr)
                                                      {
                                                          spdlog::debug("Done composing vehicle_status_intent");
                                                          std::string msg_to_pub = vsi_ptr->asJson();
                                                          this->publish_msg<const char *>(msg_to_pub.c_str(), this->_vsi_producer_worker);
                                                      }
                                                  }
                                              }
                                              else
                                              {
                                                  //Checking mobilityoperation message list. If there is no more mobilityoperation message, break the current for loop
                                                  break;
                                              }
                                          }
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
                                                 //  std::unique_lock<std::mutex> lck(worker_mtx);
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
                                              this->publish_msg<const char *>(msg_to_pub.c_str(), this->_vsi_producer_worker);
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

        bool vehicle_status_intent_service::identify_latest_mapping_bsm_mp_by_mo(std::shared_ptr<workers::bsm_worker> bsm_w_ptr,
                                                                                 std::shared_ptr<workers::mobilitypath_worker> mp_w_ptr,
                                                                                 std::shared_ptr<models::bsm> bsm_ptr,
                                                                                 std::shared_ptr<models::mobilityoperation> mo_ptr,
                                                                                 std::shared_ptr<models::mobilitypath> mp_ptr)
        {
            try
            {
                //Checking timestamp and vehicle id to find mobilitypath
                long mp_pos = 0;
                bool is_mp_mapping_found = false;
                bool is_bsm_mapping_found = false;
                spdlog::debug("Current mobilityPath list SIZE = {0}", mp_w_ptr->get_curr_list().size());
                spdlog::debug("Current BSM list SIZE = {0}", bsm_w_ptr->get_curr_list().size());
                while (mp_w_ptr && mp_ptr && mo_ptr && !mp_w_ptr->get_curr_list().empty() && mp_pos < mp_w_ptr->get_curr_list().size())
                {
                    spdlog::debug("debug mp start {0}", mp_w_ptr->get_curr_list().size());
                    std::string mp_vehicle_id = mp_w_ptr->get_curr_list().at(mp_pos).getHeader().sender_id;
                    uint64_t mp_timestamp = mp_w_ptr->get_curr_list().at(mp_pos).getHeader().timestamp;

                    //Mapping MobilityOperation and MobilityPath timestamp duration within MOBILITY_OPERATION_PATH_MAX_DURATION ms.
                    if (mo_ptr->getHeader().sender_id == mp_vehicle_id && std::abs((long)mo_ptr->getHeader().timestamp - (long)mp_timestamp) <= MOBILITY_OPERATION_PATH_MAX_DURATION)
                    {
                        spdlog::debug("debug mp during {0}", mp_w_ptr->get_curr_list().size());
                        mp_ptr->setHeader(mp_w_ptr->get_curr_list().at(mp_pos).getHeader());
                        mp_ptr->setTrajectory(mp_w_ptr->get_curr_list().at(mp_pos).getTrajectory());
                        mp_w_ptr->pop_cur_element_from_list(mp_pos); //The deque size shrik every time we call a pop element
                        is_mp_mapping_found = true;
                        spdlog::debug("debug mp end {0}", mp_w_ptr->get_curr_list().size());
                        continue;
                    }
                    mp_pos++;
                }

                //checking msg_count and BSM ID for this BSM
                long bsm_pos = 0;
                while (is_mp_mapping_found && bsm_w_ptr && bsm_ptr && mo_ptr && !bsm_w_ptr->get_curr_list().empty() && bsm_pos < bsm_w_ptr->get_curr_list().size())
                {
                    spdlog::debug("debug bsm start {0}", bsm_w_ptr->get_curr_list().size());

                    //Mapping MobilityOperation and BSM msg_count maximum allowed differences.
                    if (mo_ptr->getHeader().sender_bsm_id == bsm_w_ptr->get_curr_list().at(bsm_pos).getCore_data().temprary_id && std::abs(std::stol(mo_ptr->get_value_from_strategy_params("msg_count")) - (long)bsm_w_ptr->get_curr_list().at(bsm_pos).getCore_data().msg_count) <= MOBILITY_OPERATION_BSM_MAX_COUNT_OFFSET)
                    {
                        spdlog::debug("debug bsm during {0}", bsm_w_ptr->get_curr_list().size());
                        bsm_ptr->setCore_data(bsm_w_ptr->get_curr_list().at(bsm_pos).getCore_data());
                        bsm_w_ptr->pop_cur_element_from_list(bsm_pos); //The deque size shrik every time we call a pop element
                        is_bsm_mapping_found = true;
                        spdlog::debug("debug bsm end {0}", bsm_w_ptr->get_curr_list().size());
                        continue;
                    }
                    bsm_pos++;
                }
                return is_bsm_mapping_found && is_mp_mapping_found;
            }
            catch (...)
            {
                spdlog::critical("Identify latest mapping bsm mp and mo throw exception");
                return false;
            }
        }

        models::vehicle_status_intent vehicle_status_intent_service::compose_vehicle_status_intent(models::bsm &bsm,
                                                                                                   models::mobilityoperation &mo,
                                                                                                   models::mobilitypath &mp)
        {
            models::vehicle_status_intent vsi;
            try
            {
                vsi.setVehicle_id(mo.getHeader().sender_id);
                vsi.setDepart_position(std::stol(mo.get_value_from_strategy_params("depart_pos")));
                vsi.setCur_timestamp(mo.getHeader().timestamp);
                vsi.setIs_allowed(std::stol(mo.get_value_from_strategy_params("access")));
                vsi.setMax_accel(std::stod(mo.get_value_from_strategy_params("max_accel")));
                vsi.setMax_decel(std::stod(mo.get_value_from_strategy_params("max_decel")));
                vsi.setReact_timestamp(std::stod(mo.get_value_from_strategy_params("react_time")));
                vsi.setMinimum_gap(std::stod(mo.get_value_from_strategy_params("min_gap")));
                vsi.setDepart_position(std::stol(mo.get_value_from_strategy_params("depart_pos")));

                //Update vehicle status intent with BSM
                vsi.setVehicle_length(bsm.getCore_data().size.length);
                vsi.setCur_speed(bsm.getCore_data().speed);
                vsi.setCur_accel(bsm.getCore_data().accelSet.Long);
                std::string turn_direction = mo.get_value_from_strategy_params("turn_direction");
                vsi.SetTurn_direction(turn_direction);
                double cur_lat = bsm.getCore_data().latitude / 10000000;
                double cur_lon = bsm.getCore_data().longitude / 10000000;
                double cur_elev = bsm.getCore_data().elev;
                spdlog::debug("cur_lat = {0}", cur_lat);
                spdlog::debug("cur_lon = {0}", cur_lon);
                spdlog::debug("cur_elev = {0}", cur_elev);
                vsi.setCur_lanelet_id(_msg_lanelet2_translate_ptr->get_cur_lanelet_id_by_loc_and_direction(cur_lat, cur_lon, cur_elev, turn_direction));
                vsi.setCur_distance(_msg_lanelet2_translate_ptr->distance2_cur_lanelet_end(cur_lat, cur_lon, cur_elev, turn_direction));

                //Update vehicle status intent with MobilityPath
                models::est_path_t est_path;
                std::vector<models::est_path_t> est_path_v;
                int32_t ecef_x = mp.getTrajectory().location.ecef_x;
                int32_t ecef_y = mp.getTrajectory().location.ecef_y;
                int32_t ecef_z = mp.getTrajectory().location.ecef_z;
                long timestamp = mp.getHeader().timestamp;

                spdlog::debug("MobilityPath location ecef_x: {0}", ecef_x);
                spdlog::debug("MobilityPath location ecef_y: {0}", ecef_y);
                spdlog::debug("MobilityPath location ecef_z: {0}", ecef_z);

                est_path.distance_to_end_of_lanelet = _msg_lanelet2_translate_ptr->distance2_cur_lanelet_end(_msg_lanelet2_translate_ptr->ecef_2_map_point(ecef_x, ecef_y, ecef_z), turn_direction);
                est_path.lanelet_id = _msg_lanelet2_translate_ptr->get_cur_lanelet_id_by_point_and_direction(_msg_lanelet2_translate_ptr->ecef_2_map_point(ecef_x, ecef_y, ecef_z), turn_direction);
                est_path.timestamp = timestamp;
                est_path_v.push_back(est_path);

                spdlog::debug("MobilityPath trajectory offset size: {0}", mp.getTrajectory().offsets.size());
                message_services::models::trajectory trajectory = mp.getTrajectory();
                int32_t count = 1;
                for (auto offset_itr = trajectory.offsets.begin(); offset_itr != trajectory.offsets.end(); offset_itr++)
                {
                    count++;

                    //Allow to configure the number of mobilityPath offsets sent as part of VSI (vehicle status and intent)
                    if (this->vsi_est_path_point_count != 0 && count > this->vsi_est_path_point_count)
                    {
                        break;
                    }

                    ecef_x += offset_itr->offset_x;
                    ecef_y += offset_itr->offset_y;
                    ecef_z += offset_itr->offset_z;

                    est_path.timestamp += 100; //The duration between two points is 0.1 sec
                    est_path.distance_to_end_of_lanelet = _msg_lanelet2_translate_ptr->distance2_cur_lanelet_end(_msg_lanelet2_translate_ptr->ecef_2_map_point(ecef_x, ecef_y, ecef_z), turn_direction);
                    est_path.lanelet_id = _msg_lanelet2_translate_ptr->get_cur_lanelet_id_by_point_and_direction(_msg_lanelet2_translate_ptr->ecef_2_map_point(ecef_x, ecef_y, ecef_z), turn_direction);
                    est_path_v.push_back(est_path);
                }

                vsi.setEst_path_v(est_path_v);
                std::map<int64_t, models::intersection_lanelet_type> lanelet_id_type_m = _msg_lanelet2_translate_ptr->get_lanelet_types_ids_by_vehicle_trajectory(trajectory, vsi_est_path_point_count, turn_direction);
                for (auto itr = lanelet_id_type_m.begin(); itr != lanelet_id_type_m.end(); itr++)
                {
                    if (itr->second == models::intersection_lanelet_type::link)
                    {
                        vsi.setLink_lanelet_id(itr->first);
                    }
                    if (itr->second == models::intersection_lanelet_type::departure)
                    {
                        vsi.setDest_lanelet_id(itr->first);
                    }
                    if (itr->second == models::intersection_lanelet_type::entry)
                    {
                        vsi.setEnter_lanelet_id(itr->first);
                    }
                }
                return vsi;
            }
            catch (...)
            {
                spdlog::critical("Compose vehicle status intent Exception occur");
                return vsi;
            }
        }

        template <typename T>
        void vehicle_status_intent_service::msg_consumer(std::shared_ptr<T> msg_w_ptr, kafka_clients::kafka_consumer_worker *consumer_worker)
        {
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
            return;
        }

        template <typename T>
        void vehicle_status_intent_service::publish_msg(T msg, kafka_clients::kafka_producer_worker *producer_worker)
        {
            std::string msg_to_send = "";
            msg_to_send = (char *)msg;

            if (msg_to_send.length() > 0)
            {
                producer_worker->send(msg_to_send);
            }
            return;
        }
    }
}