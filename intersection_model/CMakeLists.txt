cmake_minimum_required(VERSION 3.10.2)
project(intersection_model)

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

find_package(PROJ4)
find_package(Eigen3 3.3 NO_MODULE)
find_package(catkin COMPONENTS REQUIRED
                lanelet2_core 
                lanelet2_projection 
                lanelet2_io )
message(STATUS "Building Library.")
add_library(${PROJECT_NAME}_lib STATIC   
                            src/intersection_model.cpp
                            src/map_msg_worker.cpp)
target_include_directories(${PROJECT_NAME}_lib PUBLIC 
                                        ${PROJECT_SOURCE_DIR}/include 
                                        ${catkin_INCLUDE_DIRS}
                                        ${catkin_LIBRARY_DIRS} )
target_link_libraries(${PROJECT_NAME}_lib PUBLIC  
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_core.so" 
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_projection.so" 
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_io.so" 
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_traffic_rules.so"
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_routing.so"
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_extension_lib.so"
                                        kafka_clients_lib 
                                        rdkafka++
                                        spdlog::spdlog
                                        Eigen3::Eigen
                                        ${catkin_LIBRARIES}) 
                                        
message(STATUS "Building Unit Tests.")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES}
                        src/intersection_model.cpp 
                        src/map_msg_worker.cpp  )
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC
                                        ${PROJECT_NAME}_lib
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_core.so" 
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_projection.so" 
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_io.so" 
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_traffic_rules.so"
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_routing.so"
                                        "${catkin_LIBRARY_DIRS}/liblanelet2_extension_lib.so"
                                        spdlog::spdlog
                                        Eigen3::Eigen
                                        ${catkin_LIBRARIES}
                                        gtest
                                        )


add_subdirectory(src/server)