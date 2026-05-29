# Finch2

## Prerequisites & Compatibility

### Minimum Required Versions

🛠️ **CMake**: 3.28+

🐂 **GCC**: 13.1+
🐉 **Clang**: 16.0+
🍏 **Apple Clang**: 15.0+
🪟 **MSVC**: 19.30+

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

### Windows

Scenario: The controller is connected to a Windows PC directly via a USB cable. The C++ code and QGroundControl are running on the same PC.

```
pip install mavproxy
mavproxy.py --master=COM3 --baudrate=921600 --out=udp:127.0.0.1:14540 --out=udp:127.0.0.1:14550
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
