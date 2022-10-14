cmake_minimum_required(VERSION 3.16)
project(streets_service_base)

include(dependencies.cmake)

add_library(streets_service_base_lib
  src/streets_configuration_exception.cpp
  src/configuration.cpp
  src/streets_configuration.cpp
)

target_compile_definitions(streets_service_base_lib
  PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
)

target_compile_options(streets_service_base_lib
  PRIVATE
    -fPIC
)

target_include_directories(streets_service_base_lib
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(streets_service_base_lib
  PUBLIC
    Boost::system
    Boost::thread
    Boost::filesystem
    spdlog::spdlog
)

########################################################
# Install streets_service_base_lib package.
########################################################
file(GLOB files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB templates ${CMAKE_CURRENT_SOURCE_DIR}/include/internal/*.tpp)

install(TARGETS streets_service_base_lib
  EXPORT streets_service_base_libTargets
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include
  ARCHIVE DESTINATION lib
)

install(EXPORT streets_service_base_libTargets
  FILE streets_service_base_libTargets.cmake
  DESTINATION lib/cmake/streets_service_base_lib/
  NAMESPACE streets_service_base_lib::
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  cmake/streets_service_base_libConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/streets_service_base_libConfig.cmake
  INSTALL_DESTINATION  lib/streets_service_base_lib/streets_service_base_lib/
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/streets_service_base_libConfig.cmake
  DESTINATION lib/cmake/streets_service_base_lib/
)

install(FILES ${files} DESTINATION include)
install(FILES ${templates} DESTINATION include/internal)

########################
# googletest for unit testing
########################
set(BINARY streets_service_base_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)

add_executable(${BINARY} ${TEST_SOURCES}
  src/streets_configuration_exception.cpp
  src/configuration.cpp
  src/streets_configuration.cpp
)

target_compile_definitions(${BINARY}
  PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_ERROR
)

target_compile_options(${BINARY}
  PRIVATE
    -pthread
)

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY}
  PUBLIC
    Boost::system
    Boost::thread
    Boost::filesystem
    spdlog::spdlog
    GTest::GTest
    streets_service_base_lib
)
