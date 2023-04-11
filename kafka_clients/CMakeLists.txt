cmake_minimum_required(VERSION 3.10.2)
project(kafka_clients)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
list(APPEND CMAKE_PREFIX_PATH "/opt/carma/cmake/")



find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog REQUIRED)
find_package(carma-clock REQUIRED)

add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

find_package(streets_service_base_lib REQUIRED)

add_library(${PROJECT_NAME}_lib STATIC 
            src/kafka_producer_worker.cpp 
            src/kafka_consumer_worker.cpp            
            src/kafka_client.cpp )


add_executable(${PROJECT_NAME}  src/main.cpp
                                src/kafka_producer_worker.cpp 
                                src/kafka_consumer_worker.cpp
                                src/kafka_client.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC 
                            Boost::system
                            Boost::thread 
                            spdlog::spdlog 
                            rdkafka++ 
                            streets_service_base_lib::streets_service_base_lib
                            ${PROJECT_NAME}_lib
                            )

target_include_directories(${PROJECT_NAME}_lib 
                           PUBLIC
                           $<INSTALL_INTERFACE:include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/src)

TARGET_LINK_LIBRARIES (${PROJECT_NAME}_lib PUBLIC Boost::system Boost::thread rdkafka++ spdlog::spdlog gmock )


#######
# Install  
#######

INSTALL(TARGETS ${PROJECT_NAME}_lib  DESTINATION lib)
FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
INSTALL(FILES ${files} DESTINATION include)


# #######################
# googletest for unit testing
# #######################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES}  
                        src/kafka_producer_worker.cpp 
                        src/kafka_consumer_worker.cpp
                        src/kafka_client.cpp)
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC 
                            Boost::system 
                            Boost::thread
                            Boost::filesystem
                            spdlog::spdlog
                            streets_service_base_lib::streets_service_base_lib 
                            rdkafka++   
                            ${PROJECT_NAME}_lib 
                            gtest)
target_include_directories(${BINARY}
                           PUBLIC
                           $<INSTALL_INTERFACE:include>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/src)
