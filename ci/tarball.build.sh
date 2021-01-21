#!/bin/bash


rm -rf .git
rm -rf *.tar.xz
find . -name '.git' -exec rm -rf {} \; || true
find . -name '.git' -exec rm -rf {} \; || true
rm -rf 3rdparty/libossia/.git || true
rm -rf 3rdparty/libossia/3rdparty/CicmWrapper || true

cp -rf 3rdparty/libossia/3rdparty/pure-data/LICENSE.txt pd-license.txt || true
cp -rf 3rdparty/libossia/3rdparty/pure-data/portaudio/portaudio/LICENSE.txt portaudio-license.txt || true
rm -rf 3rdparty/libossia/3rdparty/pure-data || true
mkdir -p 3rdparty/libossia/3rdparty/pure-data/portaudio/portaudio/ || true
mv portaudio-license.txt 3rdparty/libossia/3rdparty/pure-data/portaudio/portaudio/LICENSE.txt  || true
mv pd-license.txt 3rdparty/libossia/3rdparty/pure-data/LICENSE.txt  || true

mv 3rdparty/libossia/3rdparty/pybind11/LICENSE pybind11-license.txt || true
rm -rf 3rdparty/libossia/3rdparty/pybind11 || true
mkdir -p 3rdparty/libossia/3rdparty/pybind11 || true
mv pybind11-license.txt 3rdparty/libossia/3rdparty/pybind11/LICENSE || true

rm -rf 3rdparty/libossia/3rdparty/cpp-taskflow/benchmarks
rm -rf 3rdparty/libossia/3rdparty/cpp-taskflow/3rd-party
rm -rf 3rdparty/libossia/3rdparty/cpp-taskflow/image
rm -rf 3rdparty/libossia/3rdparty/cpp-taskflow/doxygen
rm -rf 3rdparty/libossia/3rdparty/cpp-taskflow/docs
rm -rf 3rdparty/libossia/3rdparty/r8brain-free-src/bench
rm -rf 3rdparty/libossia/3rdparty/jni_hpp
rm -rf 3rdparty/libossia/3rdparty/max-sdk
rm -rf 3rdparty/libossia/3rdparty/rapidjson/thirdparty
rm -rf 3rdparty/libossia/3rdparty/concurrentqueue/benchmarks
rm -rf 3rdparty/libossia/3rdparty/concurrentqueue/build
rm -rf 3rdparty/libossia/3rdparty/concurrentqueue/test
rm -rf 3rdparty/libossia/3rdparty/tbb/examples
rm -rf 3rdparty/libossia/3rdparty/exprtk/*test*
rm -rf 3rdparty/libossia/3rdparty/purr-data
rm -rf 3rdparty/libossia/3rdparty/boost*
rm -rf docs/Doc_sources
rm -rf docs/Doxygen
rm -rf docs/score.png
rm -rf cmake-build-*
find . -name '*.user' -exec rm {} \;
find . -name '*.user.*' -exec rm {} \;

tar caf segment.tar.xz ./*


pwd
ls