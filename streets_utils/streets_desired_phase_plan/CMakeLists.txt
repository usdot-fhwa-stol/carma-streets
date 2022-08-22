cmake_minimum_required(VERSION 3.10.2)
project(streets_desired_phase_plan)
                    
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")



find_package(Boost COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog REQUIRED)
add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

find_package(RapidJSON REQUIRED)
# Add definition for rapidjson to include std::string
add_definitions(-DRAPIDJSON_HAS_STDSTRING=1)
add_library(${PROJECT_NAME}_lib
                src/models/streets_desired_phase_plan.cpp
                src/exceptions/streets_desired_phase_plan_exception.cpp
                )


target_include_directories(${PROJECT_NAME}_lib PUBLIC
                            $<INSTALL_INTERFACE:include>
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            PRIVATE
                            ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries( ${PROJECT_NAME}_lib PUBLIC
                spdlog::spdlog
                rapidjson
                )

                

########################################################
# Install streets_desired_phase_plan package.
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
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)


add_executable(${BINARY} ${TEST_SOURCES}
                )

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} PUBLIC 
                spdlog::spdlog
                gtest
                rapidjson
                ${PROJECT_NAME}_lib
                )