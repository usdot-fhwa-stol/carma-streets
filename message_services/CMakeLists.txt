cmake_minimum_required(VERSION  3.10.2)

message(STATUS "START of message_services")

project(message_services LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

link_directories(${catkin_LIBRARY_DIRS} ${catkin_INCLUDE_DIRS})
find_package(PROJ4)
find_package(Eigen3 3.3 NO_MODULE)
find_package(catkin COMPONENTS 
                lanelet2_core 
                lanelet2_projection 
                lanelet2_io REQUIRED)
find_package( streets_service_base_lib COMPONENTS streets_service_base_lib REQUIRED)
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})

find_package(Boost COMPONENTS thread system REQUIRED)
find_package(spdlog REQUIRED)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src/models)


add_library(${PROJECT_NAME}_lib STATIC
            src/services/vehicle_status_intent_service.cpp 
            src/workers/bsm_worker.cpp 
            src/workers/mobilitypath_worker.cpp  
            src/workers/mobilityoperation_worker.cpp  
            src/workers/base_worker.cpp 
            src/models/bsm.cpp 
            src/models/mobilitypath.cpp
            src/models/mobilityoperation.cpp
            src/models/vehicle_status_intent.cpp
            src/models/baseMessage.cpp
            lib/message_lanelet2_translation.cpp
            src/exceptions/message_lanelet2_translation_exception.cpp)

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
                        Eigen3::Eigen
                        streets_service_base_lib::streets_service_base_lib
                        kafka_clients_lib 
                        rdkafka++
                        gmock
                        ${catkin_LIBRARIES}) 

foreach(_target  message_services )
    add_executable(${_target} "src/${_target}.cpp")
    target_link_libraries(${_target} PUBLIC ${PROJECT_NAME}_lib Boost::system  Boost::thread   spdlog::spdlog )
endforeach()

message(STATUS "END of message_services")



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

