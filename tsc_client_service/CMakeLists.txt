cmake_minimum_required(VERSION 3.10.2)
project(traffic_signal_controller_service)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wno-unused-variable")

set(CMAKE_CXX_STANDARD 17)
find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog REQUIRED)
# Added for using SPDLOG macros, please check SPDLOG macros logging
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
find_package(streets_service_configuration_lib REQUIRED)
find_package(streets_service_base_lib REQUIRED)
find_package(streets_signal_phase_and_timing_lib REQUIRED)
find_package(streets_tsc_configuration_lib  REQUIRED)
find_package(streets_desired_phase_plan_lib REQUIRED)
find_library(NETSNMPAGENT "netsnmpagent")
find_library(NETSNMPMIBS "netsnmpmibs")
find_library(NETSNMP "netsnmp")
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(GTest COMPONENTS GTest::gtest GTest::gmock)

list(APPEND CMAKE_PREFIX_PATH "/opt/carma/cmake/")

find_package(carma-clock REQUIRED)


add_library(${PROJECT_NAME}_lib
    src/exceptions/udp_socket_listener_exception.cpp
    src/exceptions/snmp_client_exception.cpp
    src/exceptions/monitor_states_exception
    src/exceptions/monitor_desired_phase_plan_exception.cpp
    src/intersection_client.cpp
    src/udp_socket_listener.cpp
    src/tsc_service.cpp
    src/snmp_client.cpp
    src/spat_worker.cpp
    src/monitor_tsc_state.cpp        
    src/monitor_desired_phase_plan.cpp
    src/control_tsc_state.cpp
    src/exceptions/control_tsc_state_exception.cpp)
target_link_libraries( ${PROJECT_NAME}_lib 
    PUBLIC
        streets_service_configuration_lib::streets_service_configuration_lib
        kafka_clients_lib::kafka_clients_lib
        streets_service_base_lib::streets_service_base_lib    
        streets_signal_phase_and_timing_lib
        streets_tsc_configuration_lib
        streets_desired_phase_plan_lib
        intersection_client_api_lib
        spdlog::spdlog
        Qt5::Core
        Qt5::Network
        GTest::gmock
        ::carma-clock
        ${NETSNMPAGENT} 
        ${NETSNMPMIBS} 
        ${NETSNMP}
        ${NETSNMP_LIBRARIES}
)
    
target_include_directories(${PROJECT_NAME}_lib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${NETSNMP_INCLUDE_DIRS}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable( ${PROJECT_NAME} 
        src/main.cpp
        )


target_link_libraries( ${PROJECT_NAME} 
    PUBLIC
        ${PROJECT_NAME}_lib
)



#############
## Testing ##
# #############
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${PROJECT_NAME}_test 
    ${TEST_SOURCES}
)
target_link_libraries(${PROJECT_NAME}_test 
    PUBLIC 
        ${PROJECT_NAME}_lib
        GTest::gtest
        GTest::gmock 
)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

