# Finch2

## Prerequisites & Compatibility

### Minimum Required Versions

🛠️ **CMake**: 3.28+

🐂 **GCC**: 13.1+
🐉 **Clang**: 16.0+
🍏 **Apple Clang**: 15.0+
🪟 **MSVC**: 19.30+

## Building and Running

CMake will automatically download and build **MAVSDK v3.17.1** during the first configuration step.

### Debug Build

```cpp
cmake -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug

cmake --build build-debug

./build-debug/run
```

### Release Build

For **GCC** and **Clang** (excluding macOS), the `-march=native` flag is automatically enabled.

> **Important:** A Release binary compiled with `-march=native` is locked to your specific CPU model.

```cpp
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake --build build-release

./build-release/run
```
