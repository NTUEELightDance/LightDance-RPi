import sys
import RPi.GPIO as GPIO
import time
import os

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from method import *
from pysocket import ZMQSocket

BUTTON_GPIO = 16


class Wire:
    def __init__(self) -> None:
        self.socket = ZMQSocket(port=8000)
        self.play = Play(self.socket)

    def signalHandler(self, sig, frame):
        GPIO.cleanup()
        sys.exit(0)

    def buttonPressedCallback(self, channel):
        print("Button pressed!")
        response = self.play({"start_time": 0, "delay_time": 0})
        print("Response: ", response)

    def start(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        GPIO.add_event_detect(
            BUTTON_GPIO,
            GPIO.FALLING,
            callback=self.buttonPressedCallback,
            bouncetime=100,
        )

        try:
            print("Wait for interupt")
            while True:
                print(".")
                time.sleep(1)

        except KeyboardInterrupt:
            GPIO.cleanup()  # clean up GPIO on CTRL+C exit
        GPIO.cleanup()  # clean up GPIO on normal exit


if __name__ == "__main__":
    wire = Wire()
    wire.start()
