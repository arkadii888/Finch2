cmake_minimum_required(VERSION 3.28)

project(Finch2 CXX)

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
)

target_link_libraries(run PRIVATE mavsdk)
