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
        else:
            dancerName = "10_dontstop"

        try:
            ip = get_ip()
        except socket.gaierror:
            ip = "127.0.0.1"

        hostName = get_hostname()
        return f"[ boardInfo / Success ] {ip} {hostName}"
