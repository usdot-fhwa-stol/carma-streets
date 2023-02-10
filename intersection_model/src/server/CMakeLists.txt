cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(intersection-server)


cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(qhttpengine REQUIRED)
find_package(streets_service_base_lib REQUIRED)

file(GLOB SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/handlers
)

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} PUBLIC intersection_model_lib intersection_server_api_lib Qt5Core Qt5Network ssl crypto qhttpengine streets_service_base_lib::streets_service_base_lib spdlog::spdlog)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)