import os

from .baseMethod import BaseMethod


# Reboots RPi
class Reboot(BaseMethod):
    def method(self, payload=None):
        os.system("sudo reboot -h 0")
