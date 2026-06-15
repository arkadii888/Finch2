# Finch2

## Platforms & Compatibility

🛠️ **CMake:** 3.28+

🍏 **Mac:** 
Apple Clang 15.0+ [![Status](https://github.com/arkadii888/Finch2/actions/workflows/ci.yml/badge.svg?job=macos-14%20-%20apple-clang)](https://github.com/arkadii888/Finch2/actions)

🐧 **Linux:** 
GCC 13.1+ [![Status](https://github.com/arkadii888/Finch2/actions/workflows/ci.yml/badge.svg?job=ubuntu-24.04%20-%20gcc)](https://github.com/arkadii888/Finch2/actions)
Clang 16.0+ [![Status](https://github.com/arkadii888/Finch2/actions/workflows/ci.yml/badge.svg?job=ubuntu-24.04%20-%20clang)](https://github.com/arkadii888/Finch2/actions)

## MAVProxy Setup

### Linux Edge Device

Scenario: The flight controller is connected to the Raspberry Pi via physical pins (UART). The C++ code runs directly on the Raspberry Pi. Telemetry is sent locally to the code and also transmitted via Wi-Fi to QGroundControl on an external device.

```
pip3 install mavproxy
mavproxy.py --master=/dev/ttyAMA0 --baudrate=921600 --out=udp:0.0.0.0:14540 --out=udpbcast:192.168.4.255:14550
```

### Linux PC

Scenario: The flight controller is connected to a Linux PC directly via a USB cable. The C++ code and QGroundControl are running on the same PC.

```
pip install mavproxy
mavproxy.py --master=/dev/ttyACM0 --baudrate=921600 --out=udp:127.0.0.1:14540 --out=udp:127.0.0.1:14550
```

### Mac

Scenario: The flight controller is connected to a Mac directly via a USB cable. The C++ code and QGroundControl are running on the same Mac.

```
pip3 install mavproxy
mavproxy.py --master=/dev/tty.usbmodem101 --baudrate=921600 --out=udp:127.0.0.1:14540 --out=udp:127.0.0.1:14550
```

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

### Unit Tests

Unit tests use the same CMake configure step as the main app (GoogleTest is fetched automatically on first configure). Build the `btree_test` target and run the executable:

```bash
cmake --build build-debug --target btree_test && ./build-debug/btree_test
```

## Behavior Tree

**Move** — drone mission items. Subclass `Move`. **Task** software-only action. Subclass `Task`.

To add either: implement `GetPrompt()`, `FromJson()`, and the base-class methods (`GetMissionItem`/`Validate` for moves; `Tick`/`Validate` for tasks). Register only in [`node_catalog.cpp`](src/behavior_tree/node_catalog.cpp) — add `GetPrompt()` to `GetActionPrompts()` and an `if (key == "...")` branch to `ParseActionNode()` (for moves, also push into `moves`). No changes to `btree.cpp` or `agent.cpp`.

Examples: `MoveTo` (`move_node.hpp`), `ComputeFibonacci` (`task_node.hpp`).
