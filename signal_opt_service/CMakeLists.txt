cmake_minimum_required(VERSION 3.10.2)
project(signal_opt_service)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Required for shared_mutex
set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(streets_service_base_lib COMPONENTS streets_service_base_lib REQUIRED)
find_package(streets_vehicle_list_lib COMPONENTS streets_vehicle_list_lib REQUIRED)

add_library(${PROJECT_NAME}_lib
    src/signal_opt_messages_worker.cpp
)

target_include_directories(${PROJECT_NAME}_lib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    Boost::system
    Boost::thread
    Boost::filesystem
    spdlog::spdlog
    intersection_client_api_lib
    streets_service_base_lib::streets_service_base_lib
    streets_vehicle_list_lib::streets_vehicle_list_lib
    Qt5::Core
    Qt5::Network
)

add_executable(${PROJECT_NAME} src/main.cpp src/signal_opt_service.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Network)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_lib kafka_clients_lib rdkafka++ Boost::system Boost::filesystem Boost::thread spdlog::spdlog intersection_client_api_lib streets_service_base_lib::streets_service_base_lib
    streets_vehicle_list_lib::streets_vehicle_list_lib)

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
    gtest
    Qt5::Core
    Qt5::Network
    intersection_client_api_lib
    streets_service_base_lib::streets_service_base_lib
    streets_vehicle_list_lib::streets_vehicle_list_lib
)