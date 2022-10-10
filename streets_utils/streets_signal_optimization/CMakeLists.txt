cmake_minimum_required(VERSION 3.10.2)
project(streets_signal_optimization)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Required for shared_mutex
set(CMAKE_CXX_STANDARD 17)

find_package(spdlog REQUIRED)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(streets_desired_phase_plan_lib COMPONENTS streets_desired_phase_plan_lib REQUIRED)
find_package(streets_vehicle_list_lib COMPONENTS streets_vehicle_list_lib REQUIRED)
find_package(streets_signal_phase_and_timing_lib COMPONENTS streets_signal_phase_and_timing_lib REQUIRED)
find_package(streets_vehicle_scheduler_lib COMPONENTS streets_vehicle_scheduler_lib REQUIRED)
find_package(streets_tsc_configuration_lib COMPONENTS streets_tsc_configuration_lib REQUIRED)

add_library(${PROJECT_NAME}_lib
    src/streets_desired_phase_plan_arbitrator.cpp
    src/exceptions/streets_desired_phase_plan_arbitrator_exception.cpp
)

target_include_directories(${PROJECT_NAME}_lib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME}_lib PUBLIC
    spdlog::spdlog
    streets_desired_phase_plan_lib
    intersection_client_api_lib
    streets_vehicle_list_lib
    streets_signal_phase_and_timing_lib
    streets_vehicle_scheduler_lib::streets_vehicle_scheduler_lib
    streets_tsc_configuration_lib
)

# #######################################################
# Install streets_signal_optimization package.
# #######################################################
file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB templates ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/*.tpp)

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
    INSTALL_DESTINATION lib/${PROJECT_NAME}_lib/${PROJECT_NAME}_lib/)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_libConfig.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}_lib/
)
install(FILES ${files} DESTINATION include)

# ######################
# googletest for unit testing
# ######################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test)

add_executable(${BINARY} ${TEST_SOURCES}
)

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} PUBLIC
    spdlog::spdlog
    gtest
    rapidjson
    ${PROJECT_NAME}_lib
    streets_desired_phase_plan_lib
    streets_vehicle_list_lib
    streets_signal_phase_and_timing_lib
    intersection_client_api_lib
    streets_vehicle_scheduler_lib
    streets_tsc_configuration_lib
)