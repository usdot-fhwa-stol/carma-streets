cmake_minimum_required(VERSION 3.10.2)
project(scheduling-service)

link_directories("/usr/lib"  "/usr/local/lib" )

find_package(Boost COMPONENTS thread filesystem system REQUIRED)
find_package(spdlog REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories( ${PROJECT_NAME} PUBLIC
                             ${PROJECT_SOURCE_DIR}/include 
                            "/usr/local/include")

target_link_libraries(${PROJECT_NAME} PUBLIC kafka_clients_lib rdkafka++ Boost::system  Boost::thread spdlog::spdlog)