cmake_minimum_required(VERSION 3.10.2)
project(kafka_clients)

link_directories("/usr/lib"  "/usr/local/lib" )

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)

add_library(${PROJECT_NAME}_lib STATIC 
            src/kafka_producer_worker.cpp 
            src/kafka_consumer_worker.cpp            
            src/kafka_client.cpp )

include_directories( ${PROJECT_NAME}_lib PUBLIC ${PROJECT_SOURCE_DIR}/include 
                    "/usr/local/include/librdkafka"
                    "/usr/local/include/gtest"  
                    "/usr/local/include")

add_executable(${PROJECT_NAME}  src/main.cpp
                                src/kafka_producer_worker.cpp 
                                src/kafka_consumer_worker.cpp
                                src/kafka_client.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::system  Boost::thread rdkafka++ spdlog::spdlog  ${PROJECT_NAME}_lib )

target_include_directories(${PROJECT_NAME}_lib 
                           PUBLIC
                           $<INSTALL_INTERFACE:include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/src})

TARGET_LINK_LIBRARIES (${PROJECT_NAME}_lib PUBLIC  Boost::thread rdkafka++ spdlog::spdlog)
#######
# Install  
#######

INSTALL(TARGETS ${PROJECT_NAME}_lib  DESTINATION lib)
FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
INSTALL(FILES ${files} DESTINATION include)


########################
# googletest for unit testing
########################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES}  
                        src/kafka_producer_worker.cpp 
                        src/kafka_consumer_worker.cpp
                        src/kafka_client.cpp)
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC Boost::system  Boost::thread rdkafka++ spdlog::spdlog  ${PROJECT_NAME}_lib gtest)
