include(CMakeFindDependencyMacro)

find_dependency(Boost 1.65.1 COMPONENTS system filesystem thread REQUIRED)
find_dependency(RapidJSON REQUIRED)
find_dependency(spdlog REQUIRED)
find_dependency(GTest)

include(${CMAKE_CURRENT_LIST_DIR}/streets_service_base_libTargets.cmake)
