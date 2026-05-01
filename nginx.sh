#!/usr/bin/env bash
set -e

LOCATION="$HOME/goinfre" # where to install

NGINX_VERSION=1.27.0
PREFIX="$LOCATION/nginx" 
BUILD_NGINX="$LOCATION/build_nginx"

mkdir -p $BUILD_NGINX
cd $BUILD_NGINX

# Download nginx source
if [ ! -f "nginx-$NGINX_VERSION.tar.gz" ]; then
    curl -LO http://nginx.org/download/nginx-$NGINX_VERSION.tar.gz
fi

# Extract
rm -rf "nginx-$NGINX_VERSION"
tar -xzf "nginx-$NGINX_VERSION.tar.gz"
cd "nginx-$NGINX_VERSION"

# Configure install path (no root needed)
./configure --prefix="$PREFIX" \
            --with-http_v2_module

# Build and install
make -j$(nproc)
make install

echo "✅ Nginx installed in $PREFIX"