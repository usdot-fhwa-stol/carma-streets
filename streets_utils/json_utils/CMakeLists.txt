cmake_minimum_required(VERSION 3.10.2)
project(json_utils)
# Set Flags
set(CMAKE_CXX_STANDARD 17)
# GNU standard installation directories
include(GNUInstallDirs)
# Find Packages 
find_package(RapidJSON REQUIRED)
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)

# Add definition for rapidjson to include std::string
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
set(LIBRARY_NAME ${PROJECT_NAME}_lib)
########################################################
# Build Library
######################################################## 
file(GLOB JSON_UTILS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(${LIBRARY_NAME} ${JSON_UTILS_SOURCES} )
target_include_directories(${LIBRARY_NAME} 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries( ${LIBRARY_NAME}
    PUBLIC
        rapidjson
        spdlog::spdlog
)  

########################################################
# Install streets_utils::json_utils_lib library.
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
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME})
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}Config.cmake
    DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
    DESTINATION include/streets_utils/${LIBRARY_NAME}
    FILES_MATCHING PATTERN "*.hpp" # select header files
    )
########################
# googletest for unit testing
########################
set(TEST_NAME ${PROJECT_NAME}_test)
file(GLOB_RECURSE JSON_UTILS_TEST_SOURCES LIST_DIRECTORIES false test/*.hpp test/*.cpp)
add_executable(${TEST_NAME} ${JSON_UTILS_TEST_SOURCES})
gtest_discover_tests(${TEST_NAME})
target_link_libraries(${TEST_NAME} 
    PRIVATE
        ${LIBRARY_NAME}       
)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20") 
    target_link_libraries( ${TEST_NAME} PUBLIC GTest::gtest_main )
else() 
    target_link_libraries( ${TEST_NAME} PUBLIC gtest_main )
endif()