import os
import socket
import sys

from .baseMethod import BaseMethod


# BoardInfo
class BoardInfo(BaseMethod):
    def method(self, payload):
        try:
            ip = socket.gethostbyname(socket.gethostname())
        except socket.gaierror:
            ip = "127.0.0.1"

        hostName = os.name
        return f"[ boardInfo / Success ] {ip} {hostName}"
