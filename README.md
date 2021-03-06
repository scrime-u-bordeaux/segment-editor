SEGMent Editor
=======

# V2.0 - 16/12/2019
University of Bordeaux - SCRIME

### Authors :
* Jean Michaël Celerier
* Raphaël Marczak

### Contributors :
* Frédéric Bouyssi
* Katie Brzustowski-Vaïsse
* Myriam Desainte Catherine
* Alex Fournier
* Gyorgy Kurtag Jr
* Mission d'Appui à la Pédagogie et à l'Innovation de l'Université de Bordeaux

### Funding :
* SCRIME
* IdEx Bordeaux
* Projet régional " Bibliothèques et médiation numérique au service de la réussite étudiante " (Conseil Régional Aquitaine MiPNES)




[![Build Status](https://travis-ci.org/scrime-u-bordeaux/segment-editor.svg?branch=master)](https://travis-ci.org/scrime-u-bordeaux/segment-editor)
[![Build status](https://ci.appveyor.com/api/projects/status/mmapumjj2ari9kst?svg=true)](https://ci.appveyor.com/project/JeanMichalCelerier/segment-editor)

An editor for games made with the SEGMent engine.


Building a release on windows (example):

    cmake ../segment-editor -DSEGMENT_RELEASE=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=c:/Qt/5.12.3/msvc2017_64 -DBOOST_ROOT=c:/dev/boost_1_69_0 -GNinja -DCMAKE_MAKE_PROGRAM=c:/dev/ninja.exe
    ninja
    ninja package

And on Mac, targeting 10.10 and later : 

    cmake ../segment-editor -DSEGMENT_RELEASE=1 -DCMAKE_INSTALL_PREFIX=build -DCMAKE_PREFIX_PATH=/Users/admin/Qt/5.9.7/clang_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 -GNinja 
