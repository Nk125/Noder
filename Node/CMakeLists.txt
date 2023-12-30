cmake_minimum_required(VERSION 3.19)

project(node)

set(HTTPLIB_USE_ZLIB_IF_AVAILABLE false)

find_package(OpenSSL REQUIRED)

# Every request made by the mass request should be made using async (ON) or direct non-async blocking call (OFF)?
option(SpawnAsyncHttpClient "Use the thread pool for requests" ON)

# Make a thread with the thread pool for each mass request received, else it'll make it with std::thread
option(UseThreadPoolForInit "Submit a task to the TP for each mass request" ON)

# POST Requests have a "function" made with regex that fills some data with random characters
# but regex itself waste too much resources, defaultly set as OFF
option(EnableRegexInPostRequests "Enable regex in POST Requests" OFF)

# This have a big overhead, i recommend set it to OFF
# Also doesn't work with more than 1 mass requester
option(ShowRPS "Show Requests Per Second in node" OFF)

set(ConfigFilePath "./nodeconfig.json")

message(STATUS "Configuration:")

message(STATUS "Spawn Async Client: ${SpawnAsyncHttpClient}")

message(STATUS "Use Thread Pool for Init: ${UseThreadPoolForInit}")

message(STATUS "Enable Regex in Post Requests: ${EnableRegexInPostRequests}")

message(STATUS "Show Requests Per Second?: ${ShowRPS}")

message(STATUS "Config File Path: ${ConfigFilePath}")

add_executable(node "main.cpp" "RequestHandler.cpp" "RequestSender.cpp" "Threading.cpp")

target_compile_definitions(node PUBLIC 
    CONFIG_FILE="${ConfigFilePath}"
    ASYNC_REQUEST_SEND=${SpawnAsyncHttpClient}
    USE_THREAD_POOL_FOR_INIT=${UseThreadPoolForInit}
    ENABLE_REGEX_IN_POST_REQUESTS=${EnableRegexInPostRequests}
    SHOW_RPS=${ShowRPS}
# For each mass request, init x number of clients/connections
    CONNECTIONS=1
)

set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)

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

target_compile_definitions(node PUBLIC

)

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