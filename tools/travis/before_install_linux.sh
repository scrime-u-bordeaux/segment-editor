#!/bin/bash

sudo apt-get install apt-transport-https ca-certificates gnupg software-properties-common wget

sudo add-apt-repository -y ppa:beineri/opt-qt-5.15.2-focal
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test

sudo apt-get update -qq

sudo pip install sphinx quark-sphinx-theme
