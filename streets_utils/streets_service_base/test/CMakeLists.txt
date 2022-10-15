add_executable(streets_service_base_test
  test_main.cpp
  test_streets_configuration.cpp
  test_streets_singleton.cpp
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
    streets_service_base_lib::streets_service_base_lib
)

# The CI system expects the test binary to be in the project's binary root directory
set_target_properties(streets_service_base_test
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)

gtest_discover_tests(streets_service_base_test)
