#!/bin/bash

cd /home/pi/LightDance-RPi
sudo pkill -f controller
source config
sudo ./controller/controller $DANCER_NAME
