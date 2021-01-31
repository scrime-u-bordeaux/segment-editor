#!/bin/bash
source /opt/qt*/bin/qt*-env.sh

set -eux
CC=gcc-9 CXX=g++-9 cmake -DSEGMENT_RELEASE=1 -DDEPLOYMENT_BUILD=1 -DCMAKE_INSTALL_PREFIX=appdir/usr -DCMAKE_SKIP_RPATH=ON -DCMAKE_UNITY_BUILD=1

cmake --build . -j$(nproc)
cmake --build . --target docs
cmake --build . --target install/strip/fast

find appdir/
wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod a+x linuxdeployqt-continuous-x86_64.AppImage

export VERSION="$TRAVIS_TAG"
./linuxdeployqt-continuous-x86_64.AppImage appdir/usr/share/applications/SEGMent.desktop -appimage
