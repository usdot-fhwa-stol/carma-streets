cmake_minimum_required(VERSION 3.10.2)
project(snmp_client_service)

link_directories("/usr/lib" "usr/local/lib")

find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog REQUIRED)
find_package(streets_service_base_lib COMPONENTS streets_service_base_lib REQUIRED)

add_library(${PROJECT_NAME}_lib
        src/snmp_client.cpp)

target_include_directories(${PROJECT_NAME}_lib PUBLIC
$<INSTALL_INTERFACE:include>
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries( ${PROJECT_NAME}_lib PUBLIC
Boost::system 
Boost::thread 
Boost::filesystem
spdlog::spdlog
${NETSNMP_LIBRARIES})
