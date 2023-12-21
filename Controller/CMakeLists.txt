cmake_minimum_required(VERSION 3.16)

project(node_controller)

find_package(OpenSSL REQUIRED)

include_directories("../include")

link_libraries(OpenSSL::SSL)

set(CMAKE_CXX_STANDARD 17)

add_executable(node_controller "main.cpp")