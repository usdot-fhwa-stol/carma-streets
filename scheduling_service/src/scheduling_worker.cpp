#include "scheduling_worker.h"

namespace scheduling_service{



    std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> scheduling_worker::schedule_vehicles(std::unordered_map<std::string, streets_vehicles::vehicle> veh_map, 
                                                                                                std::shared_ptr<streets_vehicle_scheduler::vehicle_scheduler> scheduler) const
    {

        std::shared_ptr<streets_vehicle_scheduler::intersection_schedule> int_schedule;
        if ( streets_service::streets_configuration::get_string_config("intersection_type").compare("stop_controlled_intersection") == 0 ) {
            int_schedule = std::make_shared<streets_vehicle_scheduler::all_stop_intersection_schedule>();
        }
        else if (streets_service::streets_configuration::get_string_config("intersection_type").compare("signalized_intersection") == 0 ) {
            int_schedule = std::make_shared<streets_vehicle_scheduler::signalized_intersection_schedule>();
        }
        else {
            SPDLOG_ERROR("Failed scheduling vehicles. Scheduling Service does not support intersection_type : {0}!", streets_service::streets_configuration::get_string_config("intersection_type"));
        }

        int_schedule->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        scheduler->schedule_vehicles(veh_map, int_schedule);

        if ( streets_service::streets_configuration::get_boolean_config("enable_schedule_logging") ) {
            auto logger = spdlog::get("csv_logger");
            if ( logger != nullptr ){
                logger->info( int_schedule->toCSV());
            }
        }

        return int_schedule;
    }

}

