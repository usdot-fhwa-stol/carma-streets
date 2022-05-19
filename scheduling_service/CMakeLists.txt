cmake_minimum_required(VERSION 3.10.2)
project(scheduling_service)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wno-unused-variable")

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)
find_package(intersection_client_api_lib REQUIRED)
find_package(streets_service_base_lib REQUIRED)


add_executable(${PROJECT_NAME}  src/main.cpp
                                src/scheduling.cpp
                                src/vehicle.cpp
                                src/intersection_client.cpp)
                                
target_include_directories(${PROJECT_NAME} PUBLIC
                                $<INSTALL_INTERFACE:include>
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                PRIVATE
                                ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PUBLIC 
                                    kafka_clients_lib 
                                    rdkafka++ 
                                    Boost::system 
                                    Boost::thread 
                                    spdlog::spdlog 
                                    intersection_client_api_lib::intersection_client_api_lib 
                                    streets_service_base_lib::streets_service_base_lib)


########################
# googletest for unit testing
########################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES}
                        src/scheduling.cpp
                        src/vehicle.cpp
                        src/intersection_client.cpp )
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} PUBLIC  
                                Boost::system 
                                Boost::filesystem 
                                kafka_clients_lib 
                                rdkafka++ Boost::thread 
                                spdlog::spdlog 
                                gtest 
                                intersection_client_api_lib::intersection_client_api_lib
                                streets_service_base_lib::streets_service_base_lib)