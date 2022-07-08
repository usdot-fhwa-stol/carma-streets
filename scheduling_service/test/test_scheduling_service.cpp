
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "intersection_client_api_lib/OAIIntersection_info.h"
#include "scheduling_service.h"

using namespace scheduling_service;


TEST(scheduling_service_test, initialization)
{
    
    int sleep_millisecs = 1000;
    int int_client_request_attempts = 10;
    
    scheduling_service::scheduling_service ss;
    ASSERT_FALSE(ss.initialize(sleep_millisecs, int_client_request_attempts));
}


TEST(scheduling_service_test, config_vehicle_list)
{
    scheduling_service::scheduling_service ss;
    
    ASSERT_TRUE(ss.config_vehicle_list());
}


TEST(scheduling_service_test, config_scheduler_without_intersection)
{
    scheduling_service::scheduling_service ss;
   
    // No intersection information
    ASSERT_FALSE(ss.config_scheduler());
}

TEST(scheduling_service_test, config_scheduler_with_intersection)
{
    scheduling_service::scheduling_service ss;
    std::string json = "{\"departure_lanelets\":[{ \"id\":162, \"length\":41.60952439839113, \"speed_limit\":11.176}, { \"id\":164, \"length\":189.44565302601367, \"speed_limit\":11.176 }, { \"id\":168, \"length\":34.130869420842046, \"speed_limit\":11.176 } ], \"entry_lanelets\":[ { \"id\":167, \"length\":195.73023157287864, \"speed_limit\":11.176 }, { \"id\":171, \"length\":34.130869411176431136, \"speed_limit\":11.176 }, { \"id\":163, \"length\":41.60952435603712, \"speed_limit\":11.176 } ], \"id\":9001, \"link_lanelets\":[ { \"conflict_lanelet_ids\":[ 161 ], \"id\":169, \"length\":15.85409574709938, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 165, 156, 161 ], \"id\":155, \"length\":16.796388658952235, \"speed_limit\":4.4704 }, { \"conflict_lanelet_ids\":[ 155, 161, 160 ], \"id\":165, \"length\":15.853947840111768943, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 155 ], \"id\":156, \"length\":9.744590320260139, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 169, 155, 165 ], \"id\":161, \"length\":16.043077028554038, \"speed_limit\":11.176 }, { \"conflict_lanelet_ids\":[ 165 ], \"id\":160, \"length\":10.295559117055083, \"speed_limit\":11.176 } ], \"name\":\"WestIntersection\"}";
    auto info =  std::make_shared<OpenAPI::OAIIntersection_info>();
    info->fromJson(QString::fromStdString(json));
    ss.set_intersection_info( info);
    // With Intersection information
    ASSERT_TRUE(ss.config_scheduler());
    
}

TEST(scheduling_service_test, config_csv_logger)
{
    scheduling_service::scheduling_service ss;
    
    // With Intersection information
    ss.configure_csv_logger();
    
}


