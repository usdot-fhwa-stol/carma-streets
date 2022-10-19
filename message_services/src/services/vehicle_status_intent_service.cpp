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
                auto client = std::make_shared<kafka_clients::kafka_client>();
                
                // kafka config
                this->bootstrap_server = streets_service::streets_configuration::get_string_config("bootstrap_server");

                // consumer topics
                this->bsm_topic_name = streets_service::streets_configuration::get_string_config("bsm_consumer_topic");
                this->bsm_group_id = streets_service::streets_configuration::get_string_config("bsm_group_id");

                this->mp_topic_name = streets_service::streets_configuration::get_string_config("mp_consumer_topic");
                this->mp_group_id = streets_service::streets_configuration::get_string_config("mp_group_id");

                this->mo_topic_name = streets_service::streets_configuration::get_string_config("mo_consumer_topic");
                this->mo_group_id = streets_service::streets_configuration::get_string_config("mo_group_id");

                // producer topics
                this->vsi_topic_name = streets_service::streets_configuration::get_string_config("vsi_producer_topic");

                _bsm_consumer_worker = client->create_consumer(this->bootstrap_server, this->bsm_topic_name, this->bsm_group_id);
                _mp_consumer_worker = client->create_consumer(this->bootstrap_server, this->mp_topic_name, this->mp_group_id);
                _mo_consumer_worker = client->create_consumer(this->bootstrap_server, this->mo_topic_name, this->mo_group_id);

                if (!_bsm_consumer_worker->init() || !_mp_consumer_worker->init() || !_mo_consumer_worker->init())
                {
                    SPDLOG_CRITICAL("kafka consumers (_bsm_consumer_worker, _mp_consumer_worker or _mo_consumer_worker) initialize error");
                }
                else
                {
                    _bsm_consumer_worker->subscribe();
                    _mp_consumer_worker->subscribe();
                    _mo_consumer_worker->subscribe();
                    if (!_bsm_consumer_worker->is_running() || !_mp_consumer_worker->is_running() || !_mo_consumer_worker->is_running())
                    {
                        SPDLOG_CRITICAL("consumer_workers (_bsm_consumer_worker, _mp_consumer_worker or _mo_consumer_worker) is not running");
                        exit(-1);
                    }
                }

                this->_vsi_producer_worker = client->create_producer(this->bootstrap_server, this->vsi_topic_name);
                if (!_vsi_producer_worker->init())
                {
                    SPDLOG_CRITICAL("kafka producer (_vsi_producer_worker) initialize error");
                    exit(-1);
                }

                this->vsi_est_path_point_count = streets_service::streets_configuration::get_int_config("vsi_est_path_count");
                this->MOBILITY_PATH_TRAJECTORY_OFFSET_DURATION = streets_service::streets_configuration::get_int_config("mobility_path_trajectory_offset_duration");
                this->VSI_TH_SLEEP_MILLI_SEC = streets_service::streets_configuration::get_int_config("vsi_th_sleep_milli_sec");
                this->BSM_MSG_EXPIRE_IN_SEC = streets_service::streets_configuration::get_int_config("bsm_msg_expire_in_sec");
                this->CLEAN_QUEUE_IN_SECS = streets_service::streets_configuration::get_int_config("clean_queue_in_secs");
                this->disable_est_path = streets_service::streets_configuration::get_boolean_config("disable_est_path");
                this->is_est_path_p2p_distance_only = streets_service::streets_configuration::get_boolean_config("is_est_path_p2p_distance_only");

                // Initialize message_lanelet2_translation
                std::string osm_file_path = streets_service::streets_configuration::get_string_config("osm_file_path");
                _msg_lanelet2_translate_ptr = std::make_shared<message_translations::message_lanelet2_translation>(osm_file_path);

                return true;
            }
            catch ( const streets_service::streets_configuration_exception &ex ) {
                SPDLOG_ERROR("Vehicle status Intent Service Initialization failure: {0} ", ex.what());
                return false;
            }
        }

        vehicle_status_intent_service::~vehicle_status_intent_service()
        {
            if (_bsm_consumer_worker)
            {
                _bsm_consumer_worker->stop();
            }

            if (_mo_consumer_worker)
            {
                _mo_consumer_worker->stop();
            }

            if (_mp_consumer_worker)
            {
                _mp_consumer_worker->stop();
            }
        }

        void vehicle_status_intent_service::start()
        {
            std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr = std::make_shared<message_services::workers::bsm_worker>();
            std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr = std::make_shared<message_services::workers::mobilitypath_worker>();
            std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr = std::make_shared<message_services::workers::mobilityoperation_worker>();
            std::shared_ptr<message_services::workers::vehicle_status_intent_worker> vsi_w_ptr = std::make_shared<message_services::workers::vehicle_status_intent_worker>();
            run(bsm_w_ptr, mp_w_ptr, mo_w_ptr);
        }

        void vehicle_status_intent_service::run(std::shared_ptr<message_services::workers::bsm_worker> bsm_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilitypath_worker> mp_w_ptr,
                                                std::shared_ptr<message_services::workers::mobilityoperation_worker> mo_w_ptr)
        {
            std::thread mp_t(&vehicle_status_intent_service::msg_consumer<workers::mobilitypath_worker>, this, std::ref(mp_w_ptr), this->_mp_consumer_worker);
            std::thread bsm_t(&vehicle_status_intent_service::msg_consumer<workers::bsm_worker>, this, std::ref(bsm_w_ptr), this->_bsm_consumer_worker);
            std::thread mo_t(&vehicle_status_intent_service::msg_consumer<workers::mobilityoperation_worker>, this, std::ref(mo_w_ptr), this->_mo_consumer_worker);

            std::shared_ptr<models::vehicle_status_intent> vsi_ptr = std::make_shared<models::vehicle_status_intent>();

            std::thread vsi_t{
                [bsm_w_ptr, mo_w_ptr, mp_w_ptr, this, vsi_ptr]()
                {
                    while (true)
                    {
                        SPDLOG_TRACE("Processing the BSM list size: {0}", bsm_w_ptr->get_curr_map().size());
                        SPDLOG_TRACE("Processing the MobilityOperation list size: {0}", mo_w_ptr->get_curr_list().size());
                        SPDLOG_TRACE("Processing the MobilityPath list size: {0}", mp_w_ptr->get_curr_map().size());
                        if (mo_w_ptr && mo_w_ptr->get_curr_list().size() > 0 && bsm_w_ptr && bsm_w_ptr->get_curr_map().size() > 0 && mp_w_ptr && mp_w_ptr->get_curr_map().size() > 0)
                        {
                            SPDLOG_TRACE("Processing the BSM, mobilityOperation and MP from list...");
                            std::unique_lock<std::mutex> lck(worker_mtx);
                            while (mo_w_ptr && !mo_w_ptr->get_curr_list().empty())
                            {
                                SPDLOG_TRACE("MO list SIZE = {0}", mo_w_ptr->get_curr_list().size());
                                SPDLOG_TRACE("MP map SIZE = {0}", mp_w_ptr->get_curr_map().size());
                                SPDLOG_TRACE("BSM map SIZE = {0}", bsm_w_ptr->get_curr_map().size());
                                message_services::models::mobilityoperation subj_mo = mo_w_ptr->get_curr_list().front();
                                mo_w_ptr->get_curr_list().pop_front();

                                message_services::models::mobilitypath subj_mp;
                                message_services::models::bsm subj_bsm;
                                bool is_bsm_msg_count_id_found = false;
                                std::time_t cur_local_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                                std::string bsm_msg_id = subj_mo.generate_hash_bsm_msg_id(subj_mo.getHeader().sender_bsm_id, std::stol(subj_mo.get_value_from_strategy_params("msg_count")), std::stol(subj_mo.get_value_from_strategy_params("sec_mark")));
                                if (bsm_w_ptr->get_curr_map().find(bsm_msg_id) != bsm_w_ptr->get_curr_map().end())
                                {
                                    subj_bsm = bsm_w_ptr->get_curr_map()[bsm_msg_id];

                                    if (std::abs(cur_local_timestamp - subj_bsm.msg_received_timestamp_) > (this->BSM_MSG_EXPIRE_IN_SEC * 1000))
                                    {
                                        SPDLOG_WARN("BSM EXPIRED {0}", std::abs(cur_local_timestamp - subj_bsm.msg_received_timestamp_));
                                        bsm_w_ptr->get_curr_map().erase(bsm_msg_id);
                                        continue;
                                    }
                                    is_bsm_msg_count_id_found = true;
                                }
                                else
                                {
                                    continue;
                                }

                                std::string sender_timestamp_msg_id = subj_mo.generate_hash_sender_timestamp_id(subj_mo.getHeader().sender_id, subj_mo.getHeader().timestamp / this->MOBILITY_OPERATION_PATH_MAX_DURATION);

                                if (is_bsm_msg_count_id_found && mp_w_ptr->get_curr_map().find(sender_timestamp_msg_id) != mp_w_ptr->get_curr_map().end())
                                {
                                    subj_mp = mp_w_ptr->get_curr_map()[sender_timestamp_msg_id];
                                }
                                else
                                {
                                    continue;
                                }

                                *vsi_ptr = compose_vehicle_status_intent(subj_bsm, subj_mo, subj_mp);
                                if (vsi_ptr)
                                {
                                    SPDLOG_DEBUG("Correlated vehicle status intent for {0}", vsi_ptr->getVehicle_id());
                                    std::string msg_to_pub = vsi_ptr->asJson();
                                    this->publish_msg<const char *>(msg_to_pub.c_str(), this->_vsi_producer_worker);
                                }
                                mp_w_ptr->get_curr_map().erase(sender_timestamp_msg_id);
                                bsm_w_ptr->get_curr_map().erase(bsm_msg_id);
                            }
                        }

                        std::time_t cur_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                        if (std::abs(cur_timestamp - this->prev_msg_expired_timestamp_) > (this->CLEAN_QUEUE_IN_SECS * 1000))
                        {
                            SPDLOG_DEBUG("Clean the BSM and MP...");
                            SPDLOG_DEBUG("MO list SIZE = {0}", mo_w_ptr->get_curr_list().size());
                            SPDLOG_DEBUG("MP map SIZE = {0}", mp_w_ptr->get_curr_map().size());
                            SPDLOG_DEBUG("BSM map SIZE = {0}", bsm_w_ptr->get_curr_map().size());

                            if (mp_w_ptr && !mp_w_ptr->get_curr_map().empty())
                            {
                                SPDLOG_DEBUG("Clean the MP...");
                                for (auto itr = mp_w_ptr->get_curr_map().cbegin(); itr != mp_w_ptr->get_curr_map().cend();)
                                {
                                    if (mp_w_ptr && std::abs(cur_timestamp - itr->second.msg_received_timestamp_) > (this->CLEAN_QUEUE_IN_SECS * 1000))
                                    {
                                        std::unique_lock<std::mutex> lck(worker_mtx);
                                        mp_w_ptr->get_curr_map().erase(itr++);
                                    }
                                    else
                                    {
                                        ++itr;
                                    }
                                }
                                SPDLOG_DEBUG("Cleaned the MP.");
                            }

                            if (bsm_w_ptr && !bsm_w_ptr->get_curr_map().empty())
                            {
                                SPDLOG_DEBUG("Clean the BSM...");
                                for (auto itr = bsm_w_ptr->get_curr_map().cbegin(); itr != bsm_w_ptr->get_curr_map().cend();)
                                {
                                    if (bsm_w_ptr && std::abs(cur_timestamp - itr->second.msg_received_timestamp_) > (this->CLEAN_QUEUE_IN_SECS * 1000))
                                    {
                                        std::unique_lock<std::mutex> lck(worker_mtx);
                                        bsm_w_ptr->get_curr_map().erase(itr++);
                                    }
                                    else
                                    {
                                        ++itr;
                                    }
                                }
                                SPDLOG_DEBUG("Cleaned the BSM.");
                            }

                            prev_msg_expired_timestamp_ = cur_timestamp;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(this->VSI_TH_SLEEP_MILLI_SEC));
                    }
                }};

            vsi_t.join();
            mp_t.join();
            mo_t.join();
            bsm_t.join();
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

                // Update vehicle status intent with BSM
                vsi.setVehicle_length(bsm.getCore_data().size.length);
                vsi.setCur_speed(bsm.getCore_data().speed);
                vsi.setCur_accel(bsm.getCore_data().accelSet.Long);
                std::string turn_direction = mo.get_value_from_strategy_params("turn_direction");
                vsi.SetTurn_direction(turn_direction);
                double cur_lat = bsm.getCore_data().latitude / 10000000;
                double cur_lon = bsm.getCore_data().longitude / 10000000;
                double cur_elev = bsm.getCore_data().elev;
                SPDLOG_DEBUG("cur_lat = {0}", cur_lat);
                SPDLOG_DEBUG("cur_lon = {0}", cur_lon);
                SPDLOG_DEBUG("cur_elev = {0}", cur_elev);

                SPDLOG_DEBUG("MobilityPath trajectory offset size: {0}", mp.getTrajectory().offsets.size());
                message_services::models::trajectory trajectory = mp.getTrajectory();
                lanelet::BasicPoint3d cur_basic_point3d = _msg_lanelet2_translate_ptr->gps_2_map_point(cur_lat, cur_lon, cur_elev);
                lanelet::Lanelet cur_lanelet = _msg_lanelet2_translate_ptr->get_cur_lanelet_by_point_and_direction(cur_basic_point3d, turn_direction, trajectory);         
                vsi.setCur_lanelet_id(cur_lanelet.id());
                vsi.setCur_distance(_msg_lanelet2_translate_ptr->distance2_cur_lanelet_end(cur_basic_point3d, cur_lanelet, turn_direction, trajectory));      
            
            if(!disable_est_path)
            {
                // Update vehicle status intent with MobilityPath
                models::est_path_t est_path;
                std::vector<models::est_path_t> est_path_v;
                int32_t ecef_x = mp.getTrajectory().location.ecef_x;
                int32_t ecef_y = mp.getTrajectory().location.ecef_y;
                int32_t ecef_z = mp.getTrajectory().location.ecef_z;
                long timestamp = mp.getHeader().timestamp;

                SPDLOG_DEBUG("MobilityPath location ecef_x: {0}", ecef_x);
                SPDLOG_DEBUG("MobilityPath location ecef_y: {0}", ecef_y);
                SPDLOG_DEBUG("MobilityPath location ecef_z: {0}", ecef_z);
                lanelet::BasicPoint3d mp_start_point = _msg_lanelet2_translate_ptr->ecef_2_map_point(ecef_x, ecef_y, ecef_z);
                if(is_est_path_p2p_distance_only)
                {
                    est_path.distance = lanelet::geometry::distance(lanelet::utils::to2D(cur_basic_point3d),lanelet::utils::to2D(mp_start_point));
                    est_path.lanelet_id = cur_lanelet.id(); //Set the lanelet id to the current vehicle lanelet id
                }
                else
                {
                    lanelet::Lanelet mp_point_lanelet = _msg_lanelet2_translate_ptr->get_cur_lanelet_by_point_and_direction(mp_start_point, turn_direction, trajectory);
                    est_path.distance = _msg_lanelet2_translate_ptr->distance2_cur_lanelet_end(mp_start_point,mp_point_lanelet, turn_direction, trajectory);
                    est_path.lanelet_id = mp_point_lanelet.id();
                }
                est_path.timestamp = timestamp;
                est_path_v.push_back(est_path);

                int32_t count = 1;
                size_t next_index = 0;
                double accumulated_distance_to_previous_point = 0;
                lanelet::BasicPoint3d mp_cur_point = mp_start_point;
                for (size_t offset_index = 0; offset_index < trajectory.offsets.size(); offset_index++)
                {
                    ecef_x += trajectory.offsets.at(offset_index).offset_x;
                    ecef_y += trajectory.offsets.at(offset_index).offset_y;
                    ecef_z += trajectory.offsets.at(offset_index).offset_z;

                    SPDLOG_DEBUG("MobilityPath location offset_x: {0}", trajectory.offsets.at(offset_index).offset_x);
                    SPDLOG_DEBUG("MobilityPath location offset_y: {0}", trajectory.offsets.at(offset_index).offset_y);
                    SPDLOG_DEBUG("MobilityPath location offset_z: {0}", trajectory.offsets.at(offset_index).offset_z);
                    est_path.timestamp += 100; // The duration between two points is 0.1 sec
                    
                    //Calculate the distance between two points (interval 0.1 secs) from the MobilityPath message starting from the current vehicle location                
                    if(is_est_path_p2p_distance_only)
                    {   
                        lanelet::BasicPoint3d mp_previous_point = mp_cur_point;
                        mp_cur_point = _msg_lanelet2_translate_ptr->ecef_2_map_point(ecef_x, ecef_y, ecef_z);
                        accumulated_distance_to_previous_point += lanelet::geometry::distance2d(lanelet::utils::to2D(mp_cur_point),lanelet::utils::to2D(mp_previous_point));                        
                    }

                    if (next_index != offset_index)
                    {
                        continue;
                    }
                    next_index += this->MOBILITY_PATH_TRAJECTORY_OFFSET_DURATION;

                    // Skip the first point
                    if (offset_index == 0)
                    {
                        continue;
                    }

                    //If the est_path only includes the distance to previous point, the distance is set to accumulated distance to previous point.
                    if(is_est_path_p2p_distance_only)
                    {
                        est_path.distance = accumulated_distance_to_previous_point;
                        est_path.lanelet_id = cur_lanelet.id(); //Set the lanelet id to the current vehicle lanelet id

                        //reset the accumulated distance, and start to calculate from the latest point
                        accumulated_distance_to_previous_point = 0;
                    }
                    else
                    {
                        lanelet::BasicPoint3d trajectory_point = _msg_lanelet2_translate_ptr->ecef_2_map_point(ecef_x, ecef_y, ecef_z);
                        lanelet::Lanelet trajectory_point_lanelet = _msg_lanelet2_translate_ptr->get_cur_lanelet_by_point_and_direction(trajectory_point, turn_direction, trajectory);            
                        est_path.distance = _msg_lanelet2_translate_ptr->distance2_cur_lanelet_end(trajectory_point, trajectory_point_lanelet, turn_direction, trajectory);
                        est_path.lanelet_id = trajectory_point_lanelet.id();
                    }
                    
                    est_path_v.push_back(est_path);
                    count++;

                    // Allow to configure the number of mobilityPath offsets sent as part of VSI (vehicle status and intent)
                    if (this->vsi_est_path_point_count != 0 && count > this->vsi_est_path_point_count)
                    {
                        break;
                    }
                }

                vsi.setEst_path_v(est_path_v);
            }
            
                std::map<int64_t, models::intersection_lanelet_type> lanelet_id_type_m = _msg_lanelet2_translate_ptr->get_lanelet_types_ids(cur_lanelet, turn_direction);
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
                if (lanelet_id_type_m.empty() ) {
                    SPDLOG_WARN("Did not find entry,link, or departure lanet for vehicle {0} in lane {1}." , vsi.getVehicle_id(), cur_lanelet.id());
                }
                return vsi;               
            }
            catch (const lanelet::LaneletError &e)
            {
                SPDLOG_ERROR("Compose vehicle status intent Exception occur : {0}!", e.what());
                return vsi;
            }
        }

        template <typename T>
        void vehicle_status_intent_service::msg_consumer(std::shared_ptr<T> msg_w_ptr, std::shared_ptr<kafka_clients::kafka_consumer_worker> consumer_worker)
        {
            while (consumer_worker->is_running())
            {
                const std::string payload = consumer_worker->consume(1000);
                if (payload.length() != 0 && msg_w_ptr)
                {
                    std::unique_lock<std::mutex> lck(worker_mtx);
                    msg_w_ptr->process_incoming_msg(payload);
                }

                if (!msg_w_ptr)
                {
                    SPDLOG_CRITICAL("Message worker is not initialized");
                }
            }
            return;
        }

        template <typename T>
        void vehicle_status_intent_service::publish_msg(T msg,  std::shared_ptr<kafka_clients::kafka_producer_worker> producer_worker)
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