cmake_minimum_required(VERSION 3.10.2)
project(streets_json_utils)
# Set Flags
set(CMAKE_CXX_STANDARD 20)
# Find Packages 
find_package(RapidJSON REQUIRED)
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)

# Add definition for rapidjson to include std::string
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
set(LIBRARY_NAME json_utils)
########################################################
# Build Library
######################################################## 
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(${LIBRARY_NAME}_lib ${SOURCES} )
target_include_directories(${LIBRARY_NAME}_lib  
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries( ${LIBRARY_NAME}_lib 
    PUBLIC
        rapidjson
        spdlog::spdlog

)  

message( STATUS "Target INCLUDE DIR : " ${INCLUDE_DIRECTORIES})

########################################################
# Install sensor_data_sharing_package package.
########################################################
file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

install(
    TARGETS ${LIBRARY_NAME}_lib
    EXPORT ${LIBRARY_NAME}_libTargets
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
)
install(
    EXPORT ${LIBRARY_NAME}_libTargets 
    FILE ${LIBRARY_NAME}_libTargets.cmake
    DESTINATION lib/cmake/${LIBRARY_NAME}_lib/
    NAMESPACE ${LIBRARY_NAME}_lib::
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${LIBRARY_NAME}_libConfig.cmake.in 
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_libConfig.cmake
    INSTALL_DESTINATION  lib/${LIBRARY_NAME}_lib/ )
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_libConfig.cmake
    DESTINATION  lib/cmake/${LIBRARY_NAME}_lib/
)
install(FILES ${files} DESTINATION include)

########################
# googletest for unit testing
########################
set(BINARY ${LIBRARY_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.hpp test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)
add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} 
    PUBLIC
        ${LIBRARY_NAME}_lib
        rapidjson
        spdlog::spdlog

 )
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20") 
    target_link_libraries( ${BINARY} PUBLIC GTest::gtest_main )
else() 
    target_link_libraries( ${BINARY} PUBLIC gtest_main )
endif()