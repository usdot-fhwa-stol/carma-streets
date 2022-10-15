cmake_minimum_required(VERSION 3.16)
project(streets_service_base)

include(GNUInstallDirs)

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
install(TARGETS streets_service_base_lib
  EXPORT streets_service_base_libTargets
  INCLUDES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT streets_service_base_libTargets
  FILE streets_service_base_libTargets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/streets_service_base_lib
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

install(DIRECTORY include
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
    PATTERN *.h
    PATTERN *.tpp
)

########################
# googletest for unit testing
########################
add_executable(streets_service_base_test
  test/test_main.cpp
  test/test_streets_configuration.cpp
  test/test_streets_singleton.cpp
)

target_compile_definitions(streets_service_base_test
  PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_ERROR
)

target_include_directories(streets_service_base_test
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(streets_service_base_test
  PRIVATE
    Boost::system
    Boost::thread
    Boost::filesystem
    spdlog::spdlog
    GTest::GTest
    streets_service_base_lib
)

gtest_discover_tests(streets_service_base_test)
