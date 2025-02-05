#!/bin/bash
#This is used for a new RPi for light dance
#Author: sallen

sudo apt update
sudo apt-get install python3-pip
sudo apt install cmake
sudo apt install git
git clone --recursive https://github.com/chousallen/LightDance-RPi.git
sudo apt install libboost-dev
sudo apt install libboost-serialization-dev
cd ~
git clone https://github.com/jgarff/rpi_ws281x.git
cd rpi_ws281x
mkdir build
cd build
cmake -D BUILD_SHARED=ON -D BUILD_TEST=OFF ..
cmake --build .
sudo make install
cd ~
./LightDance-RPi/scripts/install
cd LightDance-RPi/data
python3 gen_json.py
load
source LightDance-RPi/scripts/set-alias
