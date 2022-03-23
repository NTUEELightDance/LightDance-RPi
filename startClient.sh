#!/bin/bash

cd /home/pi/LightDance-RPi
source config
python3 ./client/client.py $DANCER_NAME
