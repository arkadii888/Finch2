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

FetchContent_MakeAvailable(mavsdk)

add_executable(run
    src/main.cpp
    src/drone.hpp
    src/px4_drone.cpp
    src/dummy_drone.cpp
    src/agent.cpp
    src/server.cpp
)

target_link_libraries(run PRIVATE mavsdk)
