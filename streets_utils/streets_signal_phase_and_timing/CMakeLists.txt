cmake_minimum_required(VERSION 3.10.2)
project(streets_signal_phase_and_timing)
########################################################
# Find Dependent Packages and set configurations
########################################################                        
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_CXX_STANDARD 17)
find_package(spdlog REQUIRED)
find_package(streets_desired_phase_plan_lib REQUIRED)
find_package(streets_tsc_configuration_lib REQUIRED)
find_package(streets_service_base_lib REQUIRED)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
find_package(RapidJSON REQUIRED)
find_package(GTest REQUIRED)
# carma-clock is installed under this directory
list(APPEND CMAKE_PREFIX_PATH "/opt/carma/cmake/")
find_package(carma-clock REQUIRED)
# Add definition for rapidjson to include std::string
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)

########################################################
# Build Library
######################################################## 
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp)
add_library(${PROJECT_NAME}_lib ${SOURCES} )
target_include_directories(${PROJECT_NAME}_lib 
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries( ${PROJECT_NAME}_lib 
    PUBLIC
        streets_service_base_lib::streets_service_base_lib
        streets_desired_phase_plan_lib::streets_desired_phase_plan_lib
        streets_tsc_configuration_lib::streets_tsc_configuration_lib
        spdlog::spdlog
        rapidjson 
)              

########################################################
# Install streets_service_base_lib package.
########################################################
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
    INSTALL_DESTINATION  lib/${PROJECT_NAME}_lib/${PROJECT_NAME}_lib/ )
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_libConfig.cmake
    DESTINATION  lib/cmake/${PROJECT_NAME}_lib/
)
install(FILES ${files} DESTINATION include)

########################
# googletest for unit testing
########################
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} 
    PUBLIC
        ${PROJECT_NAME}_lib
        rapidjson
 )
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20") 
    target_link_libraries( ${BINARY} PUBLIC GTest::gtest )
else() 
    target_link_libraries( ${BINARY} PUBLIC gtest )
endif()