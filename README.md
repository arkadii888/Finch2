# Finch2

## Platforms & Compatibility

🍏 **Mac:** 
macOS 14+

🐧 **Linux:** 
Ubuntu 24.10,
Debian 13 Trixie,
Raspberry Pi OS

<!--- [![Status](https://github.com/arkadii888/Finch2/actions/workflows/ci.yml/badge.svg?job=macos-14%20-%20apple-clang)](https://github.com/arkadii888/Finch2/actions) --->
<!--- [![Status](https://github.com/arkadii888/Finch2/actions/workflows/ci.yml/badge.svg?job=ubuntu-24.04%20-%20gcc)](https://github.com/arkadii888/Finch2/actions) --->
<!--- [![Status](https://github.com/arkadii888/Finch2/actions/workflows/ci.yml/badge.svg?job=ubuntu-24.04%20-%20clang)](https://github.com/arkadii888/Finch2/actions) --->

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

1. Install the project: 

```bash
git clone https://github.com/arkadii888/Finch2.git
cd Finch2
chmod +x setup.sh
./setup.sh
```

2. Install the model: 

```bash
cd data/models
curl -L -o Qwen3VL-8B-Instruct-Q4_K_M.gguf "https://huggingface.co/Qwen/Qwen3-VL-8B-Instruct-GGUF/resolve/main/Qwen3VL-8B-Instruct-Q4_K_M.gguf?download=true"
curl -L -o mmproj-Qwen3VL-8B-Instruct-F16.gguf "https://huggingface.co/Qwen/Qwen3-VL-8B-Instruct-GGUF/resolve/main/mmproj-Qwen3VL-8B-Instruct-F16.gguf?download=true"
```

3. Install the maps:

For elevation, use this [resource](https://portal.opentopography.org/raster?opentopoID=OTSDEM.032021.4326.3), select the area you are interested in, download and place the ```.tif``` file in ```data/maps```.

For a regular map, use this [resource](https://download.geofabrik.de/), select the area you are interested in, download and place the ```.gpkg``` in ```data/maps```

Or just download default maps for Switzerland:

```bash
cd data/maps
curl -L -o switzerland.gpkg "https://pub-3b84eed7dc5f44f6af42d2f1dd79492e.r2.dev/switzerland.gpkg"
curl -L -o switzerland.tif "https://pub-3b84eed7dc5f44f6af42d2f1dd79492e.r2.dev/switzerland.tif"
```	

4. Run:

```bash
finch
```	

## API Communication

```bash
curl -X POST http://localhost:8888/input -d "Text"
```

```bash
curl -X POST http://localhost:8888/kill
```

```bash
curl -X POST http://localhost:8888/arm
```

```bash
curl -X POST http://localhost:8888/disarm
```

```bash
curl -X POST http://localhost:8888/return
```

```bash
curl -X POST http://localhost:8888/land
```

```bash
curl http://localhost:8888/output
```

```bash
curl http://localhost:8888/telemetry
```
