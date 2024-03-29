cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

OPTION(NODEBUG  "Deactivate No debugging option" "OFF")

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fPIC -Wall -Wno-unused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++14 -Wall -Wno-unused-variable")

if(${NODEBUG} STREQUAL "OFF")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -pg -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -g3")
else (${NODEBUG} STREQUAL "OFF")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -s -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -O3")
endif(${NODEBUG} STREQUAL "OFF")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(qhttpengine REQUIRED)

file(GLOB SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/models/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/handlers/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/requests/*.cpp
)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/handlers
    ${CMAKE_CURRENT_SOURCE_DIR}/requests
)

link_directories(
    ${CMAKE_PREFIX_PATH}/lib
)

add_library(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} Qt5Core Qt5Network ssl crypto qhttpengine)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib INCLUDES DESTINATION include ARCHIVE DESTINATION lib)
FILE(GLOB include_files "${CMAKE_CURRENT_SOURCE_DIR}/*/*.h")
INSTALL(FILES ${include_files} DESTINATION include/${PROJECT_NAME})


