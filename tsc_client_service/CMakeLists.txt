cmake_minimum_required(VERSION 3.10.2)
project(traffic_signal_controller_service)


link_directories("/usr/lib" "/usr/local/lib")

find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog REQUIRED)
find_package(streets_service_base_lib COMPONENTS streets_service_base_lib REQUIRED)
find_library(NETSNMPAGENT "netsnmpagent")
find_library(NETSNMPMIBS "netsnmpmibs")
find_library(NETSNMP "netsnmp")

add_library(${PROJECT_NAME}_lib
        src/snmp_client.cpp
        src/spat_receive_worker.cpp)

add_executable( ${PROJECT_NAME} 
src/main.cpp)

target_link_libraries( ${PROJECT_NAME}_lib PUBLIC
    Boost::system 
    Boost::thread 
    Boost::filesystem
    spdlog::spdlog
    streets_service_base_lib::streets_service_base_lib
    ${NETSNMPAGENT} ${NETSNMPMIBS} ${NETSNMP}
    ${NETSNMP_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}_lib PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${NETSNMP_INCLUDE_DIRS}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

TARGET_LINK_LIBRARIES (${PROJECT_NAME}_lib PUBLIC Boost::system Boost::thread ${NETSNMP_INCLUDE_DIRS} spdlog::spdlog )
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)


#############
## Testing ##
#############
find_package(GTest REQUIRED)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
add_executable(${PROJECT_NAME}_test
    test/test_traffic_signal_controller_service
)
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_lib ${GTEST_LIBRARIES})
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

