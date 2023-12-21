cmake_minimum_required(VERSION 3.16)

project(node)

find_package(OpenSSL REQUIRED)

link_libraries(OpenSSL::SSL)

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

add_executable(node "main.cpp")

target_compile_definitions(node PUBLIC 
    CONFIG_FILE="./nodeconfig.json"
    USE_THREADING_IN_REQUESTS=UseThreadingInRequests
    USE_THREAD_POOL_FOR_REQUESTER=UseThreadPoolForRequester
    USE_THREAD_POOL_FOR_INIT=UseThreadPoolForInit
    ENABLE_REGEX_IN_POST_REQUESTS=EnableRegexInPostRequests
    REQUESTER_LOOPS=1
)

target_precompile_headers(node PUBLIC "Include.pch")

project(tester)

add_executable(tester "testDDoS.cpp")

target_include_directories(tester PUBLIC "../include")

target_compile_features(tester PUBLIC cxx_std_17)

target_compile_features(node PUBLIC cxx_std_17)