cmake_minimum_required(VERSION 3.16)

project(node_controller)

find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set (FETCHCONTENT_UPDATES_DISCONNECTED OFF)

include(FetchContent)

FetchContent_Declare(httplibgit
    GIT_REPOSITORY "https://github.com/yhirose/cpp-httplib"
    GIT_TAG "master"
    GIT_SHALLOW true
)

message(STATUS "Populating cpp-httplib")

FetchContent_MakeAvailable(httplibgit)

message(STATUS "Finished")

FetchContent_Declare(ranggit
    GIT_REPOSITORY "https://github.com/agauniyal/rang"
    GIT_TAG "master"
    GIT_SHALLOW true
)

message(STATUS "Populating rang")

FetchContent_MakeAvailable(ranggit)

message(STATUS "Finished")

FetchContent_Declare(jsongit
    URL "https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz"
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

message(STATUS "Downloading nlohmann::json v3.11.3")

FetchContent_MakeAvailable(jsongit)

message(STATUS "Finished")

add_executable(node_controller "main.cpp")

target_include_directories(node_controller PUBLIC "../include")

target_link_libraries(node_controller PUBLIC OpenSSL::SSL OpenSSL::Crypto httplib::httplib nlohmann_json::nlohmann_json rang)
