find_package(RapidJSON REQUIRED)
find_package(Boost 1.71.0 EXACT COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog 1.10.0 EXACT REQUIRED)

if(STREETSSERVICEBASE_ENABLE_TESTING OR PROJECT_IS_TOP_LEVEL)
  find_package(GTest REQUIRED)
endif()
