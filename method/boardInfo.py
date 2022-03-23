import os
import socket
import sys

from .baseMethod import BaseMethod

def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip = s.getsockname()[0]
    s.close()
    return ip

def get_hostname():
    hostname = socket.getfqdn(socket.gethostname())
    return hostname


# BoardInfo
class BoardInfo(BaseMethod):
    def method(self, payload):
        try:
            ip = socket.gethostbyname(socket.gethostname())
        except socket.gaierror:
            ip = "127.0.0.1"

        hostName = os.name
        return f"[ boardInfo / Success ] {ip} {hostName}"
