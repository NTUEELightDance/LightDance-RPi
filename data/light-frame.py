#! /usr/bin/env python3 -u

import RPi.GPIO as GPIO
import subprocess
import time
import json

GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_UP)

with open("/home/pi/LightDance-RPi/data/frame.json", "r") as f:
    control = json.load(f)

prev_state = None
while True:
    input_state = GPIO.input(17)
    print(input_state)
    if prev_state != input_state:
        if input_state == False:
            print("Button Pressed")
            for of_parts, color in control["OFPARTS"].items():
                p = subprocess.Popen(
                    f"parttest {of_parts} --hex {color}",
                    shell=True,
                    stdout=subprocess.DEVNULL,
                )
                p.wait()

            for led_parts, color in control["LEDPARTS"].items():
                p = subprocess.Popen(
                    f"parttest {led_parts} --hex {color}",
                    shell=True,
                    stdout=subprocess.DEVNULL,
                )
                p.wait()
        else:
            print("Button Released")
            p = subprocess.Popen(
                f"parttest --hex 000000",
                shell=True,
                stdout=subprocess.DEVNULL,
            )
            p.wait()
    prev_state = input_state

    time.sleep(1)
