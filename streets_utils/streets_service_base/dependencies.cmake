include(FetchContent)

FetchContent_Declare(rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG 06d58b9e848c650114556a23294d0b6440078c61  # Top-level project requires post 1.1.0 (latest release) features
)

FetchContent_GetProperties(rapidjson)
if(NOT rapidjson_POPULATED)
    FetchContent_Populate(rapidjson)

    # RapidJSON builds GTest by source, which conflicts with the source build in the top-level project. Since we do not
    # need to test RapidJSON, we disable tests, examples, and docs.

    set(RAPIDJSON_BUILD_DOC OFF CACHE INTERNAL "")
    set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(RAPIDJSON_BUILD_TESTS OFF CACHE INTERNAL "")
    set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE INTERNAL "")

    add_subdirectory(${rapidjson_SOURCE_DIR} ${rapidjson_BINARY_DIR})
endif()

find_package(Boost 1.71.0 EXACT COMPONENTS system filesystem thread REQUIRED)
find_package(spdlog 1.10.0 EXACT REQUIRED)
find_package(GTest REQUIRED MODULE)
