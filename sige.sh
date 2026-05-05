#!/usr/bin/env bash

set -e

# Config
SIEGE_VERSION="4.1.6"
PREFIX="$HOME/Desktop"
SRC_DIR="$HOME/Desktop/src"
BUILD_DIR="$SRC_DIR/siege-$SIEGE_VERSION"

echo "==> Preparing directories..."
mkdir -p "$SRC_DIR"
mkdir -p "$PREFIX"

cd "$SRC_DIR"

echo "==> Downloading Siege $SIEGE_VERSION..."
wget "https://download.joedog.org/siege/siege-$SIEGE_VERSION.tar.gz"

echo "==> Extracting..."
tar -xzf "siege-$SIEGE_VERSION.tar.gz"
cd "$BUILD_DIR"

echo "==> Configuring build (prefix=$PREFIX)..."
./configure --prefix="$PREFIX"

echo "==> Building..."
make

echo "==> Installing locally..."
make install

echo "==> Adding to PATH (temporary)..."
export PATH="$PREFIX/bin:$PATH"

echo ""
echo "==> Installation complete!"
echo "Run: $PREFIX/bin/siege --version"
echo ""
echo "To make it permanent, add this to your ~/.bashrc:"
echo 'export PATH="$HOME/local/bin:$PATH"'