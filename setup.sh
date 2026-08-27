#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "🚀 Starting clean system setup for the Finch project..."

OS=$(uname -s)
ARCH=$(uname -m)

CMAKE_COMPILER_FLAGS=()

if [ "$OS" = "Darwin" ]; then
    echo "🍏 macOS detected ($ARCH)."

    if ! command -v brew &> /dev/null; then
        echo "❌ Homebrew is not installed. Please install it first: https://brew.sh/"
        exit 1
    fi

    # CMake's C++20 module scanning needs a recent upstream Clang.  The
    # Apple-provided compiler can be too old even on otherwise supported Macs.
    echo "📦 Installing system packages via Homebrew..."
    brew install cmake ninja llvm python git

    LLVM_PREFIX="$(brew --prefix llvm)"
    export CC="$LLVM_PREFIX/bin/clang"
    export CXX="$LLVM_PREFIX/bin/clang++"
    # Homebrew's llvm-ar writes a GNU-style archive index that Apple's ld may
    # interpret as a regular member named '/'. MAVSDK builds several nested
    # static libraries, so export these as well as passing them to top-level
    # CMake.
    export AR=/usr/bin/ar
    export RANLIB=/usr/bin/ranlib
    CMAKE_COMPILER_FLAGS=(
        "-DCMAKE_TOOLCHAIN_FILE=$SCRIPT_DIR/cmake/macos-homebrew-llvm.cmake"
    )

elif [ "$OS" = "Linux" ]; then
    echo "🐧 Linux detected ($ARCH)."

    echo "📦 Installing system packages..."
    sudo apt update
    sudo apt install -y gcc-14 g++-14 ninja-build python3 python3-pip python3-venv curl wget git

    export CC=gcc-14
    export CXX=g++-14
    CMAKE_COMPILER_FLAGS=(
        "-DCMAKE_C_COMPILER=$CC"
        "-DCMAKE_CXX_COMPILER=$CXX"
    )

    CMAKE_VERSION="3.29.3"
    if ! command -v cmake &> /dev/null || [ "$(cmake --version | grep -oP '\d+\.\d+' | head -1)" \< "3.28" ]; then
        if [ "$ARCH" = "x86_64" ]; then
            CMAKE_ARCH="linux-x86_64"
        elif [ "$ARCH" = "aarch64" ]; then
            CMAKE_ARCH="linux-aarch64"
        else
            echo "❌ Unsupported Linux architecture: $ARCH"
            exit 1
        fi

        echo "🛠 Installing CMake ${CMAKE_VERSION} for ${CMAKE_ARCH}..."
        wget -qO /tmp/cmake.tar.gz "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-${CMAKE_ARCH}.tar.gz"
        sudo tar -xzf /tmp/cmake.tar.gz -C /usr/local --strip-components=1
        rm /tmp/cmake.tar.gz
    else
        echo "✅ A compatible version of CMake is already installed."
    fi

else
    echo "❌ Unsupported OS: $OS"
    exit 1
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

echo "🧹 Cleaning old build files..."
rm -rf build-release

echo "🔨 Building the Finch project (Release mode)..."
cmake -S . -B build-release -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    "${CMAKE_COMPILER_FLAGS[@]}"
cmake --build build-release

echo "🔗 Creating global 'finch' command..."
sudo mkdir -p /usr/local/bin
sudo ln -sf "$SCRIPT_DIR/build-release/run" /usr/local/bin/finch

echo "🎉 Done! The project was built successfully."
echo "👉 You can now run the program just by typing in the terminal: finch"
