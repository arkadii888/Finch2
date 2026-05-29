# Finch2

## Prerequisites & Compatibility

### Minimum Required Versions

**GCC**: 13.1+
**Clang**: 16.0+
**Apple Clang**: 15.0+
**MSVC**: 19.30+
**CMake**: 3.28+

## Building and Running

### Debug Build

```cpp
cmake -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug

cmake --build build-debug

./build-debug/run
```

### Release Build

```cpp
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake --build build-release

./build-release/run
```
