# Keep upstream Clang for C++20 modules, but use Darwin-format static archives.
# MAVSDK forwards this toolchain file to its nested third-party builds.
set(CMAKE_C_COMPILER "$ENV{CC}" CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER "$ENV{CXX}" CACHE FILEPATH "C++ compiler")
set(CMAKE_AR "/usr/bin/ar" CACHE FILEPATH "Darwin archiver" FORCE)
set(CMAKE_RANLIB "/usr/bin/ranlib" CACHE FILEPATH "Darwin archive indexer" FORCE)
