from baseMethod import BaseMethod
import os

# Reboots RPi
class Reboot(BaseMethod):
    def method(self, payload=None):
        os.system("reboot -h 0")