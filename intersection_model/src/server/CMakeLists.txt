cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(intersection-server)

include_directories("/usr/include"  "/usr/local/include")
link_directories( "/usr/lib"  "/usr/local/lib" )

add_subdirectory(src)
