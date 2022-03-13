import os
import socket
import sys

from .baseMethod import BaseMethod


# BoardInfo
class BoardInfo(BaseMethod):
    def method(self, payload):
        # info = [
        #     "boardInfo",
        #     {"name": os.name, "type": "dancer", "OK": True, "msg": "Success"},
        # ]
        # return info
        if len(sys.argv) == 2:
            dancerName = sys.argv[1]
        dancerName = "10_dontstop"
        ip = socket.gethostbyname(socket.gethostname())
        hostName = os.name
        return f"[ boardInfo / Success ] {dancerName} {ip} {hostName}"
