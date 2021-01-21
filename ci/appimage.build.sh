#!/bin/bash
export BUILD_FOLDER=/tmp/build
export SOURCE_FOLDER=$PWD

mkdir -p $BUILD_FOLDER
ln -s $BUILD_FOLDER build
docker pull ossia/score-package-linux
docker run \
           -v "$(pwd)"/CMake/Deployment/Linux/AppImage/Recipe:/Recipe \
           --mount type=bind,source="$(pwd)",target=/score \
           --mount type=bind,source="$BUILD_FOLDER",target=/build \
           ossia/score-package-linux \
           /bin/bash /Recipe

sudo chown -R $(whoami) $BUILD_FOLDER

wget "https://github.com/probonopd/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
chmod a+x appimagetool-x86_64.AppImage

wget "https://github.com/probonopd/AppImageKit/releases/download/continuous/AppRun-x86_64"
chmod a+x AppRun-x86_64
cp AppRun-x86_64 build/score.AppDir/AppRun

./appimagetool-x86_64.AppImage -n "build/score.AppDir" "Score.AppImage"

chmod a+rwx Score.AppImage
(
    cd $BUILD_FOLDER/SDK
    zip -r -q -9 $SOURCE_FOLDER/linux-sdk.zip usr
)
