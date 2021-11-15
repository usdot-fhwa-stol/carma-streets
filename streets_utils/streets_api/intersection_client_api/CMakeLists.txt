cmake_minimum_required(VERSION 3.2)

project(intersection_client_api_lib)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wno-unused-variable")
endif ()
include_directories( ${CMAKE_INCLUDE_CURRENT_DIR} )
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

find_package(Qt5Test REQUIRED)
add_library(${PROJECT_NAME}
    OAIIntersection_info.cpp
    OAILanelet_info.cpp
    OAIDefaultApi.cpp
    OAIHelpers.cpp
    OAIHttpRequest.cpp
    OAIHttpFileElement.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Network )

if(NOT APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE ssl crypto)
endif()


set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib )

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${files} DESTINATION include)