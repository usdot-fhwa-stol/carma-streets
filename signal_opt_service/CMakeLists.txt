cmake_minimum_required(VERSION 3.10.2)
project(signal_opt_service)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Required for shared_mutex
set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
find_package(RapidJSON REQUIRED)

# Add definition for rapidjson to include std::string
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(streets_service_base_lib COMPONENTS streets_service_base_lib REQUIRED)
find_package(streets_vehicle_list_lib COMPONENTS streets_vehicle_list_lib REQUIRED)
find_package(streets_signal_phase_and_timing_lib COMPONENTS streets_signal_phase_and_timing_lib REQUIRED)
find_package(streets_tsc_configuration_lib COMPONENTS streets_tsc_configuration_lib REQUIRED)
find_package(streets_desired_phase_plan_lib COMPONENTS streets_desired_phase_plan_lib REQUIRED)
find_package(streets_vehicle_scheduler_lib COMPONENTS streets_vehicle_scheduler_lib REQUIRED)
find_package(streets_signal_optimization_lib COMPONENTS streets_signal_optimization_lib REQUIRED)


add_library(${PROJECT_NAME}_lib
    src/signal_opt_service.cpp
    src/signal_opt_messages_worker.cpp
    src/signal_opt_processing_worker.cpp
)

target_include_directories(${PROJECT_NAME}_lib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    kafka_clients_lib
    rdkafka++
    Qt5::Core
    Qt5::Network
    Boost::system
    Boost::thread
    Boost::filesystem
    spdlog::spdlog
    rapidjson
    intersection_client_api_lib
    streets_signal_phase_and_timing_lib
    streets_desired_phase_plan_lib
    streets_vehicle_list_lib
    streets_vehicle_scheduler_lib
    streets_service_base_lib::streets_service_base_lib
    streets_tsc_configuration_lib
    streets_signal_optimization_lib
)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Network)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_lib kafka_clients_lib 
                                                            rdkafka++ Boost::system 
                                                            Boost::filesystem 
                                                            Boost::thread 
                                                            spdlog::spdlog 
                                                            rapidjson 
                                                            intersection_client_api_lib 
                                                            streets_service_base_lib 
                                                            streets_vehicle_scheduler_lib
                                                            streets_signal_phase_and_timing_lib
                                                            streets_vehicle_list_lib
                                                            streets_signal_optimization_lib)

# #######################
# googletest for unit testing
# #######################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} PUBLIC
    ${PROJECT_NAME}_lib
    Boost::system
    Boost::thread
    Boost::filesystem
    spdlog::spdlog
    rapidjson
    gtest
    gmock
    Qt5::Core
    Qt5::Network
    intersection_client_api_lib
    streets_signal_optimization_lib
    streets_vehicle_scheduler_lib
    streets_service_base_lib
    streets_vehicle_list_lib
    streets_signal_phase_and_timing_lib
    streets_tsc_configuration_lib
    streets_desired_phase_plan_lib
)