cmake_minimum_required(VERSION 3.2)

project(intersection_client_api)
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
add_library(${PROJECT_NAME}_lib
    src/OAIIntersection_info.cpp
    src/OAILanelet_info.cpp
    src/OAIDefaultApi.cpp
    src/OAIHelpers.cpp
    src/OAIHttpRequest.cpp
    src/OAIHttpFileElement.cpp
)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC Qt5::Core Qt5::Network )

if(NOT APPLE)
  target_link_libraries(${PROJECT_NAME}_lib PUBLIC ssl crypto)
endif()


set_property(TARGET ${PROJECT_NAME}_lib PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME}_lib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME}_lib PROPERTY CXX_EXTENSIONS OFF)

########################################################
# Install intersection_client_api package.
########################################################
file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

install(
    TARGETS ${PROJECT_NAME}_lib
    EXPORT ${PROJECT_NAME}_libTargets
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
)
install(
    EXPORT ${PROJECT_NAME}_libTargets 
    FILE ${PROJECT_NAME}_libTargets.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}_lib/
    NAMESPACE ${PROJECT_NAME}_lib::
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${PROJECT_NAME}_libConfig.cmake.in 
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_libConfig.cmake
    INSTALL_DESTINATION  lib/${PROJECT_NAME}_lib/${PROJECT_NAME}_lib/ )
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_libConfig.cmake
    DESTINATION  lib/cmake/${PROJECT_NAME}_lib/
)

install(FILES ${files} DESTINATION include)