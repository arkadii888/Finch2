# Finch2

**Build & Run Debug** 

```cpp
cmake -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug

cmake --build build-debug

./build-debug/run
```

**Build & Run Release** 

```cpp
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release

cmake --build build-release

./build-release/run
```
