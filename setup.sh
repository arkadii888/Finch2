#!/bin/bash
set -e

echo "🚀 Starting clean system setup for the Finch..."

echo "📦 Installing system packages and GCC-13..."
sudo apt update
sudo apt install -y gcc-13 g++-13 ninja-build python3 curl wget git

export CC=gcc-13
export CXX=g++-13

CMAKE_VERSION="3.29.3"
if ! command -v cmake &> /dev/null || [ "$(cmake --version | grep -oP '\d+\.\d+' | head -1)" \< "3.28" ]; then
    echo "🛠 Installing CMake ${CMAKE_VERSION}..."
    wget -qO /tmp/cmake.tar.gz "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-aarch64.tar.gz"
    sudo tar -xzf /tmp/cmake.tar.gz -C /usr/local --strip-components=1
    rm /tmp/cmake.tar.gz
else
    echo "✅ A compatible version of CMake is already installed."
fi

if ! command -v uv &> /dev/null; then
    echo "🐍 Installing uv..."
    curl -LsSf https://astral.sh/uv/install.sh | sh
    export PATH="$HOME/.local/bin:$PATH"
else
    echo "✅ uv is already installed."
fi

echo "📦 Synchronizing Python packages for tools/map_renderer..."
uv sync --project tools/map_renderer

echo "🔨 Building the Finch project (Release mode)..."
cmake -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release

echo "🔗 Creating global 'finch' command..."
sudo ln -sf "$(pwd)/build-release/run" /usr/local/bin/finch

echo "🎉 Done! The project was built successfully."
echo "👉 You can now run the program just by typing in the terminal: finch"
