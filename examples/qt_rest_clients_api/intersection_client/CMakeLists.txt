cmake_minimum_required(VERSION 3.2)

project(intersection_client)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wno-unused-variable")

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

include_directories("/usr/local/include")

add_executable(${PROJECT_NAME}
    main.cpp
    intersection_client.cpp
    )
target_link_libraries(${PROJECT_NAME} PRIVATE intersection_client_api_lib)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Network )