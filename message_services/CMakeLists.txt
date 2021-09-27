cmake_minimum_required(VERSION  3.10.2)

message(STATUS "START of message_services")

project(message_services LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
link_directories("/usr/lib"  "/usr/local/lib" )
message(STATUS ${CMAKE_CURRENT_BINARY_DIR})

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)
include_directories("/usr/local/include")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src/models)

add_library(${PROJECT_NAME}_lib 
            src/services/vehicle_status_intent_service.cpp 
            src/workers/bsm_worker.cpp 
            src/workers/mobilitypath_worker.cpp  
            src/workers/mobilityoperation_worker.cpp 
            src/workers/vehicle_status_intent_worker.cpp 
            src/workers/base_worker.cpp 
            src/models/bsm.cpp 
            src/models/mobilitypath.cpp
            src/models/mobilityoperation.cpp
            src/models/vehicle_status_intent.cpp
            src/models/objBase.cpp)
            
foreach(_target  message_services )
    add_executable(${_target} "src/${_target}.cpp")
    target_link_libraries(${_target} PUBLIC ${PROJECT_NAME}_lib kafka_clients_lib rdkafka++ Boost::system  Boost::thread   spdlog::spdlog )
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

