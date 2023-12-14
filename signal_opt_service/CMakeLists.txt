cmake_minimum_required(VERSION 3.10.2)
project(signal_opt_service)
########################################################
# Find Dependent Packages and set configurations
########################################################  
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
# Required for shared_mutex
set(CMAKE_CXX_STANDARD 17)
find_package(spdlog REQUIRED)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
find_package(RapidJSON REQUIRED)
# Add definition for rapidjson to include std::string
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(streets_service_configuration_lib REQUIRED)
find_package(streets_vehicle_list_lib REQUIRED)
find_package(streets_signal_phase_and_timing_lib REQUIRED)
find_package(streets_tsc_configuration_lib REQUIRED)
find_package(streets_desired_phase_plan_lib REQUIRED)
find_package(streets_vehicle_scheduler_lib REQUIRED)
find_package(streets_signal_optimization_lib  REQUIRED)
list(APPEND CMAKE_PREFIX_PATH "/opt/carma/cmake/")
find_package(carma-clock REQUIRED)

########################################################
# Build Library and Executable
######################################################## 
add_library(${PROJECT_NAME}_lib
    src/signal_opt_service.cpp
    src/signal_opt_messages_worker.cpp
    src/signal_opt_processing_worker.cpp
)
target_include_directories(${PROJECT_NAME}_lib 
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME}_lib 
    PUBLIC
        streets_service_configuration_lib::streets_service_configuration_lib
        streets_signal_phase_and_timing_lib::streets_signal_phase_and_timing_lib
        streets_desired_phase_plan_lib::streets_desired_phase_plan_lib
        streets_vehicle_list_lib::streets_vehicle_list_lib
        streets_vehicle_scheduler_lib::streets_vehicle_scheduler_lib
        streets_tsc_configuration_lib::streets_tsc_configuration_lib
        streets_signal_optimization_lib::streets_signal_optimization_lib
        kafka_clients_lib::kafka_clients_lib
        ::carma-clock
        Qt5::Core
        Qt5::Network
        spdlog::spdlog
        rapidjson
        intersection_client_api_lib  
)
add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} 
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_lib )

# #######################
# googletest for unit testing
# #######################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} 
    PRIVATE
        ${PROJECT_NAME}_lib
        spdlog::spdlog
        rapidjson
)
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20") 
    target_link_libraries( ${BINARY} PUBLIC GTest::gtest GTest::gmock )
else() 
    target_link_libraries( ${BINARY} PUBLIC gtest gmock)
endif()