cmake_minimum_required(VERSION 3.28)

project(Finch2 CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${CMAKE_SOURCE_DIR}/compile_commands.json"
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        AND NOT APPLE)

    set(CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} -march=native"
        )
  endif()
endif()

add_compile_options(-Wno-address-of-packed-member)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  mavsdk
  GIT_REPOSITORY https://github.com/mavlink/MAVSDK.git
  GIT_TAG v3.17.1
)

FetchContent_Declare(
  cpp_httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.46.0
)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
set(JSON_BuildTests OFF CACHE INTERNAL "")

FetchContent_Declare(
  llama_cpp
  GIT_REPOSITORY https://github.com/ggerganov/llama.cpp.git
  GIT_TAG b9442
)
set(LLAMA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_COMMON ON CACHE BOOL "" FORCE)
set(LLAMA_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(LLAMA_BUILD_SERVER ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.17.0
)
set(SPDLOG_BUILD_TESTS OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_EXAMPLE OFF CACHE INTERNAL "")
set(SPDLOG_BUILD_COMPILED ON CACHE INTERNAL "")

FetchContent_MakeAvailable(mavsdk cpp_httplib nlohmann_json llama_cpp spdlog)

add_executable(run
    src/main.cpp

    src/drone/drone.hpp
    src/drone/dummy_drone.cpp
    src/drone/px4_drone.cpp

    src/llm_service/llm_service.hpp
    src/llm_service/llama_service.cpp

    src/agent.cpp
    src/server.cpp
    src/logger.hpp
    src/config.hpp
)

target_link_libraries(run PRIVATE
    mavsdk
    httplib
    nlohmann_json::nlohmann_json
    spdlog
)

if (TARGET llama-server)
  target_compile_definitions(run PRIVATE
        LLAMA_SERVER_PATH="$<TARGET_FILE:llama-server>"
    )
endif()

target_compile_definitions(run PRIVATE
    PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
