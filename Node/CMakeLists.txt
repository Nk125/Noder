cmake_minimum_required(VERSION 3.19)

project(node)

find_package(OpenSSL REQUIRED)

# Use threads for mass request in server
set (UseThreadingInRequests true)

if (UseThreadingInRequests) 
    # Use the thread pool for requests (This consumes too much RAM, i usually set this to false)
    set (UseThreadPoolForRequester false)
else()
    # Don't touch here, if you set it to true it has no effect, it's just to fill the var
    set (UseThreadPoolForRequester false)
endif()

# Make a thread with the thread pool for each mass request received, else it'll make it with std::thread
set (UseThreadPoolForInit false)

# POST Requests have a "function" made with regex that fills some data with random characters, but waste too much performance so this is setal
set (EnableRegexInPostRequests false)

message(STATUS "Configuration:")

message(STATUS "Use Threading in Requests: ${UseThreadingInRequests}")

message(STATUS "Use thread pool For Requester: ${UseThreadPoolForRequester}")

message(STATUS "Use thread pool for init: ${UseThreadPoolForInit}")

message(STATUS "Enable Regex in Post Requests: ${EnableRegexInPostRequests}")

add_executable(node "main.cpp")

target_compile_definitions(node PUBLIC 
    CONFIG_FILE="./nodeconfig.json"
    USE_THREADING_IN_REQUESTS=UseThreadingInRequests
    USE_THREAD_POOL_FOR_REQUESTER=UseThreadPoolForRequester
    USE_THREAD_POOL_FOR_INIT=UseThreadPoolForInit
    ENABLE_REGEX_IN_POST_REQUESTS=EnableRegexInPostRequests
    REQUESTER_LOOPS=1
)

set (FETCHCONTENT_UPDATES_DISCONNECTED ON)

include(FetchContent)

FetchContent_Declare(httplibgit
    GIT_REPOSITORY "https://github.com/yhirose/cpp-httplib"
    GIT_TAG "master"
    GIT_SHALLOW true
)

message(STATUS "Populating cpp-httplib")

FetchContent_MakeAvailable(httplibgit)

message(STATUS "Finished")

FetchContent_Declare(jsongit
    URL "https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz"
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

message(STATUS "Downloading nlohmann::json v3.11.3")

FetchContent_MakeAvailable(jsongit)

message(STATUS "Finished")

FetchContent_Declare(tpgit
    GIT_REPOSITORY "https://github.com/bshoshany/thread-pool"
    GIT_TAG "master"
    GIT_SHALLOW true
)

message(STATUS "Populating BS::Thread Pool")

FetchContent_GetProperties(tpgit)

if (NOT ${tpgit_POPULATED})
    FetchContent_Populate(tpgit)
endif()

target_include_directories(node PUBLIC "${tpgit_SOURCE_DIR}/include")

message(STATUS "Finished")

project(tester)

add_executable(tester "testDDoS.cpp")

target_include_directories(tester PUBLIC "../include")

target_include_directories(node PUBLIC "../include")

target_compile_features(tester PUBLIC cxx_std_17)

target_compile_features(node PUBLIC cxx_std_17)

target_link_libraries(node PUBLIC httplib::httplib nlohmann_json::nlohmann_json OpenSSL::SSL OpenSSL::Crypto)

target_link_libraries(tester PUBLIC httplib::httplib OpenSSL::SSL OpenSSL::Crypto)

target_precompile_headers(node PUBLIC "../include/Include.pch")