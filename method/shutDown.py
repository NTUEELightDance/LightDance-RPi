import os

from .baseMethod import BaseMethod


# Shutdown RPi
class ShutDown(BaseMethod):
    def method(self, payload=None):
        os.system("sudo shutdown -h 0")
