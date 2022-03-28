import os
import sys
import time

import RPi.GPIO as GPIO

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from method import *
from pysocket import ZMQSocket

BUTTON_GPIO = 16

def log(msg):
    print(msg)
    sys.stdout.flush()

class Wire:
    def __init__(self) -> None:
        self.socket = ZMQSocket(port=8000)
        
        self.play = Play(self.socket)
        self.load =Load(self.socket)
        self.restartController = RestartController(self.socket)
        
        self.restartController()
        time.sleep(10) # wait for controller to restart
        self.load({"path": "./data/"})


    def signalHandler(self, sig, frame):
        GPIO.cleanup()
        sys.exit(0)

    def button_pressed_down_callback(self, channel):   # 軟體組從這裡開始加要開始的東東
        # clean event listener
        GPIO.remove_event_detect(BUTTON_GPIO)
        # start listenting falling event
        self.recv_time = time.time()
        GPIO.add_event_detect(BUTTON_GPIO, GPIO.FALLING, callback=self.button_pressed_up_callback, bouncetime=100)


    def button_pressed_up_callback(self, channel):
        # Trigger only when delta_T is between 0.47 second ~ 0.53 second (the siganal should be at 0.5 second)
        if (time.time() - self.recv_time > 0.47 and time.time() - self.recv_time < 0.53):
            log("Sending Play request ...")
            response = self.play({"start_time": 0, "delay_time": 0})
            log("Response: " + response)

        # clean event listener and start listenting rising event
        GPIO.remove_event_detect(BUTTON_GPIO)
        GPIO.add_event_detect(BUTTON_GPIO, GPIO.RISING, callback=self.button_pressed_down_callback, bouncetime=100)

    def start(self):
        # Initialize the gpio pin to low and start listenting rising event
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
        GPIO.add_event_detect(BUTTON_GPIO, GPIO.RISING, callback=self.button_pressed_down_callback, bouncetime=100)

        try:
            log("Wait for interrupt")
            while True:
                time.sleep(0.1)

        except KeyboardInterrupt:
            # GPIO.cleanup()  # clean up GPIO on CTRL+C exit
            log("Exit")
        finally: 
            GPIO.cleanup()  # clean up GPIO on normal exit


if __name__ == "__main__":
    wire = Wire()
    wire.start()
