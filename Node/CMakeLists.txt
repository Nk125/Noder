cmake_minimum_required(VERSION 3.20)

project(node)

find_package(OpenSSL REQUIRED)

include_directories("../include")

link_libraries(OpenSSL::SSL)

set(CMAKE_CXX_STANDARD 17)

add_executable(node "main.cpp")

project(tester)

add_executable(tester "testDDoS.cpp")