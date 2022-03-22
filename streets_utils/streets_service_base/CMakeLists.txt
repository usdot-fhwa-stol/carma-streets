cmake_minimum_required(VERSION 3.10.2)
project(streets_service_base)
                    
link_directories("/usr/lib"  "/usr/local/lib"  )

find_package(Boost COMPONENTS system thread REQUIRED)
find_package(spdlog REQUIRED)

include_directories("/usr/local/include")

add_library(${PROJECT_NAME}_lib 
                src/streets_configuration_exception.cpp
                src/streets_configuration.cpp
                )
target_include_directories( ${PROJECT_NAME}_lib PUBLIC ${PROJECT_SOURCE_DIR}/include 
                "/usr/local/include")
target_link_libraries( ${PROJECT_NAME}_lib Boost::system Boost::thread spdlog::spdlog )

                

#######
# Install  
#######

INSTALL(TARGETS ${PROJECT_NAME}_lib  DESTINATION lib)
FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
INSTALL(FILES ${files} DESTINATION include)


########################
# googletest for unit testing
########################
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(BINARY ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false test/*.h test/*.cpp)
set(SOURCES ${TEST_SOURCES} WORKING_DIRECTORY  ${PROJECT_SOURCE_DIR}/test)


add_executable(${BINARY} ${TEST_SOURCES} 
                    src/streets_configuration_exception.cpp
                    src/streets_configuration.cpp
                    )

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_include_directories(${BINARY} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${BINARY} PUBLIC Boost::system Boost::thread spdlog::spdlog gtest ${PROJECT_NAME}_lib)