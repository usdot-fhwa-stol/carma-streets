cmake_minimum_required(VERSION 3.10.2)
project(streets_vehicle_list)
                    
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(spdlog REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(GTest REQUIRED)

add_library(${PROJECT_NAME}_lib
                src/models/vehicle_list.cpp
                src/exceptions/status_intent_processing_exception.cpp
                src/message_processors/all_stop_status_intent_processor.cpp
                src/message_processors/status_intent_processor.cpp
                )

target_link_libraries(${PROJECT_NAME}_lib PUBLIC spdlog::spdlog rapidjson)
target_include_directories(${PROJECT_NAME}_lib PUBLIC
                            $<INSTALL_INTERFACE:include>
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            PRIVATE
                            ${CMAKE_CURRENT_SOURCE_DIR}/src)
########################
# googletest for unit testing
########################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)


add_executable(${BINARY} ${TEST_SOURCES} 
            src/models/vehicle_list.cpp
            src/exceptions/status_intent_processing_exception.cpp
            src/message_processors/all_stop_status_intent_processor.cpp
            src/message_processors/status_intent_processor.cpp
                )

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} PUBLIC  
                spdlog::spdlog
                gtest 
                ${PROJECT_NAME}_lib
                )
