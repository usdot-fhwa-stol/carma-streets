cmake_minimum_required(VERSION 3.10.2)
project(traffic_signal_controller_service)


find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog REQUIRED)
# Added for using SPDLOG macros, please check SPDLOG macros logging
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
find_package(streets_service_base_lib COMPONENTS streets_service_base_lib REQUIRED)
find_package(streets_signal_phase_and_timing_lib COMPONENTS streets_signal_phase_and_timing_lib REQUIRED)
find_library(NETSNMPAGENT "netsnmpagent")
find_library(NETSNMPMIBS "netsnmpmibs")
find_library(NETSNMP "netsnmp")
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

add_library(${PROJECT_NAME}_lib
        src/exceptions/spat_worker_exception.cpp
        src/snmp_client.cpp
        src/spat_worker.cpp
        src/monitor_tsc_state.cpp)

add_executable( ${PROJECT_NAME} 
        src/main.cpp)


target_link_libraries( ${PROJECT_NAME}_lib PUBLIC
    Boost::system 
    Boost::thread 
    Boost::filesystem
    spdlog::spdlog
    streets_service_base_lib::streets_service_base_lib
    streets_signal_phase_and_timing_lib
    intersection_client_api_lib
    Qt5::Core
    Qt5::Network
    ${NETSNMPAGENT} ${NETSNMPMIBS} ${NETSNMP}
    ${NETSNMP_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}_lib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${NETSNMP_INCLUDE_DIRS}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries( ${PROJECT_NAME} PUBLIC 
    kafka_clients_lib
    ${PROJECT_NAME}_lib)

#############
## Testing ##
#############
find_package(GTest REQUIRED)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${PROJECT_NAME}_test 
    ${TEST_SOURCES}
)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_lib gtest gmock)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

