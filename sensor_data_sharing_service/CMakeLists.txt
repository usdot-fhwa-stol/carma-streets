#  Copyright 2019-2023 Leidos
# 
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
# 
#      http:www.apache.org/licenses/LICENSE-2.0
# 
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
cmake_minimum_required(VERSION  3.10.2)
project(sensor_data_sharing_service LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
# GNU standard installation directories
include(GNUInstallDirs)
# Find Packages 
find_package(spdlog REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(GTest REQUIRED)
find_package(streets_messages_lib REQUIRED)
find_package(streets_service_base_lib REQUIRED)
list(APPEND CMAKE_PREFIX_PATH "/opt/carma/cmake/")

find_package(carma-clock REQUIRED)
# Add definition for rapidjson to include std::string
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

set(SERVICE_NAME ${PROJECT_NAME})
########################################################
# Build Library
######################################################## 
file(GLOB SENSOR_DATA_SHARING_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${SERVICE_NAME} ${SENSOR_DATA_SHARING_SOURCES} )
target_include_directories(${SERVICE_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries( ${SERVICE_NAME}
    PUBLIC
        spdlog::spdlog
        rapidjson 
        streets_service_base_lib::streets_service_base_lib
        streets_utils::streets_messages_lib
)  

########################
# Setup Test executable
########################
set(TEST_NAME ${SERVICE_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.hpp test/*.cpp)
set(SOURCES ${SERVICE_NAME} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${TEST_NAME} ${TEST_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
target_link_libraries(${TEST_NAME} 
    PRIVATE
        ${LIBRARY_NAME}
        GTest::Main
 )
