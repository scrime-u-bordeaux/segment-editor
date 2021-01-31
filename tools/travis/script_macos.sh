#!/bin/bash -eux
export QT_PATH="$PWD/qt-5.9.7"

mkdir build
cd build

cmake .. \
    -DSEGMENT_RELEASE=1 \
    -DDEPLOYMENT_BUILD=1 \
    -DCMAKE_INSTALL_PREFIX=inst \
    -DCMAKE_SKIP_RPATH=ON \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DCMAKE_PREFIX_PATH=$QT_PATH \
    -DCMAKE_UNITY_BUILD=1

cmake --build . -j$(nproc)
cmake --build . --target docs
cmake --build . --target install/strip/fast

find inst/
