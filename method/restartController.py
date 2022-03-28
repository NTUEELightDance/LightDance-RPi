import os

from .baseMethod import BaseMethod


# Reboots RPi
class RestartController(BaseMethod):
    def method(self, payload=None):
        os.system("sudo systemctl restart controller")
