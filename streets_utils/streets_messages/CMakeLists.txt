cmake_minimum_required(VERSION 3.10.2)
project(streets_messages)
# Set Flags
set(CMAKE_CXX_STANDARD 17)
# Find Packages 
find_package(spdlog REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(GTest REQUIRED)

find_package(json_utils_lib REQUIRED)
# Add definition for rapidjson to include std::string
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
set(LIBRARY_NAME ${PROJECT_NAME}_lib)
########################################################
# Build Library
######################################################## 
file(GLOB STREETS_MESSAGES_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp)
add_library(${LIBRARY_NAME} ${STREETS_MESSAGES_SOURCES} )
target_include_directories(${LIBRARY_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries( ${LIBRARY_NAME} 
    PUBLIC
        spdlog::spdlog
        rapidjson 
        streets_utils::json_utils_lib
)  
########################################################
# Install streets_messages package.
########################################################
install(
    TARGETS ${LIBRARY_NAME}
    EXPORT ${LIBRARY_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/streets_utils/
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/streets_utils/${LIBRARY_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/streets_utils/
)
install(
    EXPORT ${LIBRARY_NAME}Targets 
    FILE ${LIBRARY_NAME}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE streets_utils::
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${LIBRARY_NAME}Config.cmake.in 
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake
    INSTALL_DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME})
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake
    DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include/streets_utils/${LIBRARY_NAME}
    FILES_MATCHING PATTERN "*.hpp" # select header files
    )

########################
# googletest for unit testing
########################
set(TEST_NAME ${LIBRARY_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.hpp test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${TEST_NAME} ${TEST_SOURCES})
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
target_link_libraries(${TEST_NAME} 
    PRIVATE
        ${LIBRARY_NAME}
        rapidjson
        GTest::Main
 )
