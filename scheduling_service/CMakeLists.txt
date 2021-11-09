cmake_minimum_required(VERSION 3.10.2)
project(scheduling_service)

link_directories("/usr/lib"  "/usr/local/lib" )

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)

add_library(${PROJECT_NAME}_lib STATIC 
            src/configuration.cpp 
            src/vehicle.cpp            
            src/osm.cpp
            src/scheduling.cpp )

include_directories( ${PROJECT_NAME}_lib PUBLIC ${PROJECT_SOURCE_DIR}/include 
                    "/usr/local/include/librdkafka"
                    "/usr/local/include/gtest"  
                    "/usr/local/include")

add_executable(${PROJECT_NAME}  src/main.cpp
                                src/configuration.cpp
                                src/osm.cpp
                                src/scheduling.cpp
                                src/vehicle.cpp    )
target_include_directories( ${PROJECT_NAME} PUBLIC
                            ${PROJECT_SOURCE_DIR}/include 
                            "/usr/local/include"
                            "/usr/local/include/librdkafka"
                            "/usr/local/include/gtest")

target_link_libraries(${PROJECT_NAME} PUBLIC kafka_clients_lib rdkafka++ Boost::system  Boost::thread spdlog::spdlog)


########################
# googletest for unit testing
########################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC  ${PROJECT_NAME}_lib Boost::system kafka_clients_lib rdkafka++ Boost::thread spdlog::spdlog gtest)