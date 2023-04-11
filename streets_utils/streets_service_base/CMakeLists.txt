cmake_minimum_required(VERSION 3.10.2)
project(streets_service_base)
                    
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
list(APPEND CMAKE_PREFIX_PATH "/opt/carma/cmake/")

find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog REQUIRED)
find_package(carma-clock REQUIRED)

add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


add_library(${PROJECT_NAME}_lib ${SOURCES} )


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
                ::carma-clock
                rdkafka++
                kafka_clients_lib
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
install(FILES ${templates} DESTINATION include/internal)


########################
# googletest for unit testing
########################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/test)


add_executable(${BINARY} ${TEST_SOURCES}
                src/streets_configuration_exception.cpp
                src/configuration.cpp
                src/streets_configuration.cpp )

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} PUBLIC 
                Boost::system 
                Boost::thread
                Boost::filesystem 
                spdlog::spdlog
                gtest 
                ::carma-clock
                rdkafka++
                kafka_clients_lib
                ${PROJECT_NAME}_lib
                )