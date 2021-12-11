from baseMethod import BaseMethod
import os

# Shutdown RPi
class ShutDown(BaseMethod):
    def method(self, payload=None):
        os.system("sudo halt")