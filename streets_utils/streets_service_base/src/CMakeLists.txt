include(GNUInstallDirs)

set(STREETSSERVICEBASE_SPDLOG_LEVEL SPDLOG_LEVEL_WARN CACHE STRING "Logging level for spdlog")
set_property(CACHE STREETSSERVICEBASE_SPDLOG_LEVEL PROPERTY STRINGS
  SPDLOG_LEVEL_TRACE
  SPDLOG_LEVEL_DEBUG
  SPDLOG_LEVEL_INFO
  SPDLOG_LEVEL_WARN
  SPDLOG_LEVEL_ERROR
  SPDLOG_LEVEL_CRITICAL
  SPDLOG_LEVEL_OFF
)

add_library(streets_service_base_lib
  streets_configuration_exception.cpp
  configuration.cpp
  streets_configuration.cpp
)

add_library(streets_service_base_lib::streets_service_base_lib ALIAS streets_service_base_lib)

target_compile_definitions(streets_service_base_lib
  PRIVATE
    SPDLOG_ACTIVE_LEVEL=${STREETSSERVICEBASE_SPDLOG_LEVEL}
)

target_compile_options(streets_service_base_lib
  PRIVATE
    -fPIC
)

target_include_directories(streets_service_base_lib
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(streets_service_base_lib
  PUBLIC
    Boost::system
    Boost::thread
    Boost::filesystem
    spdlog::spdlog
)

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

install(FILES ${PROJECT_SOURCE_DIR}/cmake/streets_service_base_libConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/streets_service_base_lib/
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING
    PATTERN *.h
    PATTERN *.tpp
)
