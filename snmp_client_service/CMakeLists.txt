cmake_minimum_required(VERSION 3.10.2)
project(traffic_signal_controller_service)


# link_directories("/usr/lib" "/usr/local/lib")

find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog REQUIRED)
find_package(streets_service_base_lib COMPONENTS streets_service_base_lib REQUIRED)
find_library(NETSNMPAGENT "netsnmpagent")
find_library(NETSNMPMIBS "netsnmpmibs")
find_library(NETSNMP "netsnmp")

add_library(${PROJECT_NAME}_lib
        src/snmp_client.cpp
        src/main.cpp)

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


# INSTALL(TARGETS ${PROJECT_NAME}_lib  DESTINATION lib)
# FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
# INSTALL(FILES ${files} DESTINATION include)

install(
    TARGETS ${PROJECT_NAME}_lib
    EXPORT ${PROJECT_NAME}_libTargets
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
)

target_link_libraries(${PROJECT_NAME}
${PROJECT_NAME}_lib )


# add_dependencies(${PROJECT_NAME}_exec ${PROJECT_NAME}_lib)
# target_link_libraries( ${PROJECT_NAME}_exec PUBLIC
#     ${PROJECT_NAME}_lib 
#     Boost::system 
#     Boost::thread 
#     Boost::filesystem
#     spdlog::spdlog 
#     ${NETSNMP_LIBRARIES})
