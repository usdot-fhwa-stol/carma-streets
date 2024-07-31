#include "tsc_service.h"

namespace traffic_signal_controller_service {
    
    std::mutex dpp_mtx;
    std::mutex snmp_cmd_queue_mtx;
    using namespace streets_service;
    bool tsc_service::initialize() {
        if (!streets_service::initialize()) {
            return false;
        }
        try
        {
            // Initialize SNMP Client
            std::string target_ip = streets_configuration::get_string_config("target_ip");
            int target_port = streets_configuration::get_int_config("target_port");
            std::string community = streets_configuration::get_string_config("community");
            int snmp_version = streets_configuration::get_int_config("snmp_version");
            int timeout = streets_configuration::get_int_config("snmp_timeout");
            if (!snmp_client_ptr && !initialize_snmp_client(target_ip, target_port, community, snmp_version, timeout)) {    
                SPDLOG_ERROR("Failed to initialize snmp_client!");
                return false;
            }
            
            //  Initialize tsc configuration state kafka producer
            std::string tsc_config_topic_name = streets_configuration::get_string_config("tsc_config_producer_topic");
            if (!tsc_config_producer && !initialize_kafka_producer( tsc_config_topic_name, tsc_config_producer)) {

                SPDLOG_ERROR("Failed to initialize kafka tsc_config_producer!");
                return false;
            }

            //Initialize TSC State
            if (!initialize_tsc_state(snmp_client_ptr)){
                SPDLOG_ERROR("Failed to initialize tsc state");
                return false;
            }
            tsc_config_state_ptr = tsc_state_ptr->get_tsc_config_state();

            // Intialize spat kafka producer
            std::string spat_topic_name = streets_configuration::get_string_config("spat_producer_topic");
            if ( !spat_producer && !initialize_kafka_producer( spat_topic_name, spat_producer) ) {
                
                SPDLOG_ERROR("Failed to initialize kafka spat_producer!");
                return false;
                
            }

            std::string dpp_consumer_topic = streets_configuration::get_string_config("desired_phase_plan_consumer_topic");
            // TODO: Remove if no longer necessary for TM/TSP MMITSS Integration
            // use_mmitss_mrp = streets_configuration::get_boolean_config("use_mmitss_mrp"); 
            // -----------------------------------------------------------------------------
            auto spat_projection_int = streets_configuration::get_int_config("spat_projection_mode");
            spat_proj_mode = spat_projection_mode_from_int(spat_projection_int);
            // TODO: Remove if no longer necessary for TM/TSP MMITSS Integration
            // Phase control schedule(PCS) consumer topic
            // std::string pcs_consumer_topic = streets_configuration::get_string_config("phase_control_schedule_consumer_topic");
            // -----------------------------------------------------------------------------

            // Initialize DPP consumer and monitor
            if ( spat_proj_mode == SPAT_PROJECTION_MODE::DPP_PROJECTION ) {
                if (!desired_phase_plan_consumer && !initialize_kafka_consumer( dpp_consumer_topic, desired_phase_plan_consumer)) {
                    SPDLOG_ERROR("Failed to initialize kafka desired_phase_plan_consumer!");
                    return false;
                }
                monitor_dpp_ptr = std::make_shared<monitor_desired_phase_plan>( snmp_client_ptr );
                control_tsc_state_sleep_dur_ = streets_configuration::get_int_config("control_tsc_state_sleep_duration");
                // Initialize control_tsc_state ptr
                control_tsc_state_ptr_ = std::make_shared<control_tsc_state>(snmp_client_ptr, tsc_state_ptr);

            }
           
            // TODO: Remove if no longer necessary for TM/TSP MMITSS Integration
            // if( SPAT_PROJECTION_MODE::MMITSS_PHASE_SCHEDULE)
            //     if ( !phase_control_schedule_consumer && !initialize_kafka_consumer(pcs_consumer_topic, phase_control_schedule_consumer)){
            //         SPDLOG_ERROR("Failed to initialize kafka phase_control_schedule_consumer!");
            //         return false;
            //     }
            //      //Initialize phase control schedule
            //     phase_control_schedule_ptr = std::make_shared<streets_phase_control_schedule::streets_phase_control_schedule>();
            //     // Initialize control_tsc_state ptr
            //     control_tsc_state_sleep_dur_ = streets_configuration::get_int_config("control_tsc_state_sleep_duration");
            //     control_tsc_state_ptr_ = std::make_shared<control_tsc_state>(snmp_client_ptr, tsc_state_ptr);      
            // }
            // ------------------------------------------------------------------------------------------------------------------
            
            // Initialize spat_worker
            std::string socket_ip = streets_configuration::get_string_config("udp_socket_ip");
            int socket_port = streets_configuration::get_int_config("udp_socket_port");
            int socket_timeout = streets_configuration::get_int_config("socket_timeout");
            bool use_msg_timestamp =  streets_configuration::get_boolean_config("use_tsc_timestamp");         
            enable_snmp_cmd_logging_ = streets_configuration::get_boolean_config("enable_snmp_cmd_logging");
            if (!initialize_spat_worker(socket_ip, socket_port, socket_timeout, use_msg_timestamp)) {
                SPDLOG_ERROR("Failed to initialize SPaT Worker");
                return false;
            }

            // Initialize intersection_client
            if (!initialize_intersection_client()) {
                SPDLOG_ERROR("Failed to initialize intersection client");
                return false;
            }
            // Add all phases to a single map
            auto all_phases = tsc_state_ptr->get_vehicle_phase_map();
            auto ped_phases = tsc_state_ptr->get_ped_phase_map();
            // Insert pedestrian phases into map of vehicle phases.
            all_phases.insert(ped_phases.begin(), ped_phases.end());
            // Initialize spat ptr
            initialize_spat(intersection_client_ptr->get_intersection_name(), intersection_client_ptr->get_intersection_id(), 
                                all_phases);
            
            if (enable_snmp_cmd_logging_)
            {
                configure_snmp_cmd_logger();
                configure_veh_ped_call_logger();
            }
            if (is_simulation_mode()) {
                // Trigger spat broadcasting for EVC on startup.
                enable_spat();
            }
            SPDLOG_INFO("Traffic Signal Controller Service initialized successfully!");
            return true;
        }
        catch (const streets_configuration_exception &ex)
        {
            SPDLOG_ERROR("Signal Optimization Service Initialization failure: {0} ", ex.what());
            return false;
        }
    }

    bool tsc_service::initialize_snmp_client(const std::string &server_ip, const int server_port, const std::string &community,
                                        const int snmp_version, const int timeout) {
        try {
            snmp_client_ptr = std::make_shared<snmp_client>(server_ip, server_port, community, snmp_version, timeout);
            SPDLOG_DEBUG("SNMP Client initialized!");
            return true;
        }
        catch (const snmp_client_exception &e) {
            SPDLOG_ERROR("Exception encountered initializing snmp client : \n {0}", e.what());
            return false;
        }
    }

    bool tsc_service::initialize_tsc_state(const std::shared_ptr<snmp_client> _snmp_client_ptr ) {
        tsc_state_ptr = std::make_shared<tsc_state>(_snmp_client_ptr );
        if ( !tsc_state_ptr->initialize() ){
            SPDLOG_ERROR("Failed to initialize tsc_state!");
            return false;
        }
        SPDLOG_DEBUG("TSC State initialized!");
        return true;
    }
    bool tsc_service::enable_spat() const{
        // Enable SPaT 
        streets_snmp_cmd::snmp_response_obj enable_spat;
        enable_spat.type = streets_snmp_cmd::RESPONSE_TYPE::INTEGER;
        enable_spat.val_int = 2;
        if (!snmp_client_ptr->process_snmp_request(ntcip_oids::ENABLE_SPAT_OID, streets_snmp_cmd::REQUEST_TYPE::SET, enable_spat)){
            SPDLOG_ERROR("Failed to enable SPaT broadcasting on Traffic Signal Controller!");
            return false;
        }
        SPDLOG_DEBUG("Enabled UDP broadcasting of NTCIP SPaT data from TSC!");
        return true;
    }

    bool tsc_service::initialize_spat_worker(const std::string &udp_socket_ip, const int udp_socket_port, 
                                        const int timeout, const bool use_tsc_timestamp) {
        spat_worker_ptr = std::make_shared<spat_worker>(udp_socket_ip, udp_socket_port, timeout, use_tsc_timestamp);
        // HTTP request to update intersection information
        if (!spat_worker_ptr->initialize())
        {
            SPDLOG_ERROR("Failed to initialize spat worker!");
            return false;
        }
        SPDLOG_DEBUG("Spat worker initialized successfully!");
        return true;
    }

    bool tsc_service::initialize_intersection_client() {
        intersection_client_ptr = std::make_shared<intersection_client>();
        if ( !intersection_client_ptr->request_intersection_info() ) {
            SPDLOG_ERROR("Failed to retrieve intersection information from intersection model!");
            return false;
        }
        SPDLOG_DEBUG("Intersection client initialized successfully!");
        return true;
    }
    
    void tsc_service::initialize_spat(const std::string &intersection_name, const int intersection_id, 
                                const std::unordered_map<int,int> &phase_number_to_signal_group) {
        spat_ptr =  std::make_shared<signal_phase_and_timing::spat>();
        spat_ptr->initialize_intersection( intersection_name, intersection_id, phase_number_to_signal_group);
    }


    void tsc_service::produce_spat_json() const {
        try {
            int count = 0;
            uint64_t spat_processing_time = 0;
            SPDLOG_DEBUG("Starting SPaT producer");
            while(spat_worker_ptr && tsc_state_ptr && spat_producer) {
                try {
                    spat_worker_ptr->receive_spat(spat_ptr);
                    SPDLOG_DEBUG("Current SPaT : {0} ", spat_ptr->toJson());  
                    switch (spat_proj_mode)
                    {
                        case SPAT_PROJECTION_MODE::FIXED_TIMING_PROJECTION: {
                            tsc_state_ptr->add_future_movement_events(spat_ptr);
                            break;
                        }
                        case SPAT_PROJECTION_MODE::DPP_PROJECTION : {
                            std::scoped_lock<std::mutex> lck{dpp_mtx};
                            // throws desired phase plan exception when no previous green information present
                            monitor_dpp_ptr->update_spat_future_movement_events(spat_ptr, tsc_state_ptr); 
                            break;
                        }
                        default: {
                            // Poll and log vehicle and pedestrian call information every 10 spat messages or at 10 Hz
                            if ( count%10 == 0) {
                                tsc_state_ptr->poll_vehicle_pedestrian_calls();
                                if(auto logger = spdlog::get(VEH_PED_CALL_LOGGER_NAME); logger != nullptr ){
                                    logger->info("{0}, {1}, {2}", streets_clock_singleton::time_in_ms(), 
                                                    tsc_state_ptr->vector_to_string(tsc_state_ptr->get_vehicle_calls()),  
                                                    tsc_state_ptr->vector_to_string(tsc_state_ptr->get_pedestrian_calls())
                                                );
                                }
                            }
                        }
                    } 
                    if (spat_ptr && spat_producer) {
                        spat_producer->send(spat_ptr->toJson());
                        // No utility in calculating spat latency in simulation mode
                        if ( !this->is_simulation_mode() ) {
                           log_spat_latency(count, spat_processing_time, spat_ptr->get_intersection().get_epoch_timestamp()) ;
                        }
                    }     
                }
                catch( const signal_phase_and_timing::signal_phase_and_timing_exception &e ) {
                    SPDLOG_ERROR("Encountered exception, spat not published : \n {0}", e.what());
                }
                catch( const traffic_signal_controller_service::monitor_desired_phase_plan_exception &e) {
                    SPDLOG_ERROR("Could not update movement events, spat not published. Encountered exception : \n {0}", e.what());
                }
                catch(const traffic_signal_controller_service::monitor_states_exception &e){
                    SPDLOG_ERROR("Could not update movement events, spat not published. Encountered exception : \n {0}", e.what());
                }   
            } 
            SPDLOG_WARN("Stopping produce_spat_json! Are pointers null: spat_worker {0}, spat_producer {1}, tsc_state {2}",
                spat_worker_ptr == nullptr, spat_ptr == nullptr, tsc_state_ptr == nullptr);
        }
        catch( const udp_socket_listener_exception &e) {
            SPDLOG_ERROR("Encountered exception : \n {0}", e.what());
        }
        
    }

    void tsc_service::produce_tsc_config_json() const{
        try {
            
            while(tsc_config_producer->is_running() && tsc_config_state_ptr )
            { 
                tsc_config_producer->send(tsc_config_state_ptr->toJson());
                streets_clock_singleton::sleep_for(10000); // Sleep for 10 second between publish   
            }
        }
        catch( const streets_tsc_configuration::tsc_configuration_state_exception &e) {
            SPDLOG_ERROR("Encountered exception : \n {0}", e.what());
        }
    }

    void tsc_service::consume_desired_phase_plan() {
        desired_phase_plan_consumer->subscribe();
        while (desired_phase_plan_consumer->is_running())
        {
            const std::string payload = desired_phase_plan_consumer->consume(1000);
            if (payload.length() != 0)
            {
                SPDLOG_DEBUG("Consumed: {0}", payload);
                std::scoped_lock<std::mutex> lck{dpp_mtx};
                monitor_dpp_ptr->update_desired_phase_plan(payload);
                
                // update command queue
                if(monitor_dpp_ptr->get_desired_phase_plan_ptr()){
                    // Send desired phase plan to control_tsc_state
                    std::scoped_lock<std::mutex> snmp_cmd_lck(snmp_cmd_queue_mtx);
                    control_tsc_state_ptr_->update_tsc_control_queue(monitor_dpp_ptr->get_desired_phase_plan_ptr(), tsc_set_command_queue_);
                    
                }
            }

        }        
    }
    // TODO: Remove if no longer necessary for TM/TSP MMITSS Integration
    // void tsc_service::consume_phase_control_schedule(){
    //     phase_control_schedule_consumer->subscribe();
    //     while (phase_control_schedule_consumer->is_running())
    //     {
    //         const std::string payload = phase_control_schedule_consumer->consume(1000);
    //         if (payload.length() != 0)
    //         {
    //             SPDLOG_DEBUG("Consumed: {0}", payload);
    //             try {
    //                 //Update phase control schedule with the latest incoming schedule
    //                 phase_control_schedule_ptr->fromJson(payload);
    //                 //Update command queue
    //                 std::scoped_lock<std::mutex> snmp_cmd_lck(snmp_cmd_queue_mtx);
    //                 control_tsc_state_ptr_->update_tsc_control_queue(phase_control_schedule_ptr, tsc_set_command_queue_);
    //             } catch(streets_phase_control_schedule::streets_phase_control_schedule_exception &ex){
    //                 SPDLOG_DEBUG("Failed to consume phase control schedule commands: {0}", ex.what());
    //             }                
    //         }
    //     }  
    // }
    // --------------------------------------------------------------------------------------------

    void tsc_service::control_tsc_phases()
    {
        SPDLOG_INFO("Starting TSC Control Phases");        
        try
        {
            while(true)
            {
                SPDLOG_INFO("Iterate TSC Control Phases for time {0}", streets_clock_singleton::time_in_ms());
                set_tsc_hold_and_omit_forceoff_call();
                streets_clock_singleton::sleep_for(control_tsc_state_sleep_dur_);
            }
        }
        catch(const control_tsc_state_exception &e){
            SPDLOG_ERROR("Encountered exception : \n {0}", e.what());
            if(!tsc_set_command_queue_.empty())
            {
                SPDLOG_ERROR("Removing front command from queue :  {0}", tsc_set_command_queue_.front().get_cmd_info());
                std::scoped_lock<std::mutex> snmp_cmd_lck(snmp_cmd_queue_mtx);
                tsc_set_command_queue_.pop();
            }
        }
    }
    
    void tsc_service::set_tsc_hold_and_omit_forceoff_call()
    {
        if(control_tsc_state_ptr_ && !tsc_set_command_queue_.empty())
        {
            //Clear all phase controls from the traffic signal controller
            SPDLOG_DEBUG("Clear all phase controls from TSC!");
            control_tsc_state_ptr_->run_clear_all_snmp_commands();
        }
        while(!tsc_set_command_queue_.empty())
        {   
            auto current_command = tsc_set_command_queue_.front();            
            SPDLOG_DEBUG("Checking if front command {0} is expired", current_command.get_cmd_info());
            //Check if event is expired
            long duration = current_command.start_time_ - streets_clock_singleton::time_in_ms();
            if(duration < 0){
                throw control_tsc_state_exception("SNMP set command is expired! Start time was " 
                    + std::to_string(current_command.start_time_) + " and current time is " 
                    + std::to_string(streets_clock_singleton::time_in_ms() ) + ".");
            }       
            SPDLOG_DEBUG("Sleeping for {0}ms.", duration);
            streets_clock_singleton::sleep_for(duration);
            
            //After sleep duration, check the snmp queue again. If empty, stop the current command execution             
            if(tsc_set_command_queue_.empty())
            {
                SPDLOG_DEBUG("SNMP command queue is empty, stop the current command execution: {0}", current_command.get_cmd_info());
                break;
            }
            //Check if queue commands has been updated
            else if (tsc_set_command_queue_.front().get_cmd_info() != current_command.get_cmd_info())
            {
                SPDLOG_DEBUG("SNMP command queue is updated with new schedule, stop the current command execution: {0}", current_command.get_cmd_info());
                break;
            }

            if(!control_tsc_state_ptr_)
            {
                throw control_tsc_state_exception("Control TSC state is not initialized.");
            }

            //Checking all the commands from the queue. If there are any commands executed at the same time as the current_command, and execute these commands now.
            while (!tsc_set_command_queue_.empty())
            {   
                auto command_to_execute = tsc_set_command_queue_.front();
                if(command_to_execute.start_time_ == current_command.start_time_)
                {
                    if(control_tsc_state_ptr_ && !control_tsc_state_ptr_->run_snmp_cmd_set_request(command_to_execute))
                    {
                        throw control_tsc_state_exception("Could not set state for movement group in desired phase plan/phase control schedule.");
                    }
                    // Log command info sent
                    SPDLOG_INFO(command_to_execute.get_cmd_info());                
                    if ( enable_snmp_cmd_logging_ ){
                        if(auto logger = spdlog::get(SNMP_COMMAND_LOGGER_NAME); logger != nullptr ){
                            logger->info( command_to_execute.get_cmd_info());
                        }
                    }
                    //SNMP command Queue has no update nor has been cleared, pop the existing front command.
                    std::scoped_lock<std::mutex> snmp_cmd_lck(snmp_cmd_queue_mtx); 
                    tsc_set_command_queue_.pop();
                }
                else
                { 
                    break;
                }                                
            }
        }
    }

    void tsc_service::configure_snmp_cmd_logger() const
    {
        try {
            create_daily_logger(SNMP_COMMAND_LOGGER_NAME, ".log", "[%H:%M:%S:%e ] %v", spdlog::level::info );
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            SPDLOG_ERROR( "SNMP Command Logger initialization failed: {0}!",ex.what());
        }
    }

     void tsc_service::configure_veh_ped_call_logger() const
    {
        try {
            auto veh_ped_call_logger = create_daily_logger(VEH_PED_CALL_LOGGER_NAME, ".csv", "%v", spdlog::level::info );
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            SPDLOG_ERROR( "Vehicle Pedestrian Call Logger initialization failed: {0}!",ex.what());
        }
        // TODO: Figure out how to termine if a new file was created or an existing file appended and only write column headers on new files
        // veh_ped_call_logger->info("Timestamp (ms), Vehicle Calls (Signal Group ID), Pedestrian Calls (Signal Group ID)");
    }
    void  tsc_service::log_spat_latency(int &count, uint64_t &spat_processing_time, uint64_t spat_time_stamp) const {
        // Calculate and average spat processing time over 20 messages sent 
        if (count <= 20 ) {
            uint64_t timestamp = streets_clock_singleton::time_in_ms();
            spat_processing_time += timestamp - spat_time_stamp;
            count++;
        }
        // Log result at 20 and clear counts  
        if (count >= 20 ) {
            double total_processing_time = ((double)(spat_processing_time))/20.0;
            SPDLOG_INFO("SPat average processing time over 20 messages is {0} ms and total processing time for 20 messages is {1} ms!", total_processing_time, spat_processing_time);
            spat_processing_time = 0;
            count = 0;
        }
    } 
    

    void tsc_service::start() {
        streets_service::streets_service::start();
        SPDLOG_INFO("Starting TSC Service threads");
        std::thread tsc_config_thread(&tsc_service::produce_tsc_config_json, this);

        std::thread spat_t(&tsc_service::produce_spat_json, this);

        if ( spat_proj_mode == SPAT_PROJECTION_MODE::DPP_PROJECTION ) {
            // Run thread to consume dpp
            std::thread desired_phase_plan_t(&tsc_service::consume_desired_phase_plan, this);
            // Run tsc control phases
            std::thread control_phases_t(&tsc_service::control_tsc_phases, this);
            SPDLOG_DEBUG("Thread joined to consume desired phase plan generated by carma-streets SO service and send SNMP commands");

            desired_phase_plan_t.join();
            control_phases_t.join();
        }
        // TODO: Remove if no longer necessary for TM/TSP MMITSS Integration
        // An indicator to control whether launching a thread to consume carma-streets internal desired phase plan generated by singal optimization (SO) service 
        // or a thread to consume MMITSS external phase control schedule generated by MRP (https://github.com/mmitss/mmitss-az)
        // if ( spat_proj_mode == SPAT_PROJECTION_MODE::MMITSS_PHASE_SCHEDULE ) {
        //     std::thread consume_phase_control_schedule_t(&tsc_service::consume_phase_control_schedule, this);
        //     SPDLOG_DEBUG("Thread joined to consume phase control schedule generated by MRP.");
        //     consume_phase_control_schedule_t.join();
        //     // Run tsc control phases
        //     std::thread control_phases_t(&tsc_service::control_tsc_phases, this);
        //     control_phases_t.join();
        // }
        // ---------------------------------------------------------------------------------------------------------------
        
        // Run threads as joint so that they dont overlap execution 
        tsc_config_thread.join();
        spat_t.join();
        
    }
    

    tsc_service::~tsc_service()
    {
        if (spat_producer)
        {
            SPDLOG_WARN("Stopping spat producer!");
            spat_producer->stop();
        }

        if(tsc_config_producer)
        {
            SPDLOG_WARN("Stopping tsc config producer!");
            tsc_config_producer->stop();
        }
        if (desired_phase_plan_consumer) {
            SPDLOG_WARN("Stopping desired phase plan consumer!");
            desired_phase_plan_consumer->stop();
        }

        if (phase_control_schedule_consumer) {
            SPDLOG_WARN("Stopping phase control consumer!");
            phase_control_schedule_consumer->stop();
        }
    }
}