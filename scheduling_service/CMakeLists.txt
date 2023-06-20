cmake_minimum_required(VERSION 3.10.2)
project(scheduling_service)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wno-unused-variable")
set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(streets_service_configuration_lib REQUIRED)
find_package(streets_service_base_lib REQUIRED)
find_package(streets_vehicle_list_lib REQUIRED)
find_package(streets_vehicle_scheduler_lib REQUIRED)
find_package(streets_signal_phase_and_timing_lib REQUIRED)
find_package(kafka_clients_lib REQUIRED)
list(APPEND CMAKE_PREFIX_PATH "/opt/carma/cmake/")
find_package(carma-clock REQUIRED)

add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)


add_library(${PROJECT_NAME}_lib src/intersection_client.cpp
                                src/scheduling_service.cpp
                                src/scheduling_worker.cpp)
                                
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
        streets_service_base_lib::streets_service_base_lib
        kafka_clients_lib::kafka_clients_lib
        Boost::system 
        Boost::filesystem 
        spdlog::spdlog 
        intersection_client_api_lib 
        ::carma-clock  
        streets_vehicle_list_lib::streets_vehicle_list_lib 
        streets_vehicle_scheduler_lib::streets_vehicle_scheduler_lib
        Qt5::Core 
        Qt5::Network 
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

########################
# googletest for unit testing
########################
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} 
    PUBLIC 
        ${PROJECT_NAME}_lib  
        GTest::gtest
)    