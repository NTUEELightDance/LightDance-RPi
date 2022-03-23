import os

# import sys
import socket
from datetime import datetime

# sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/ntp")
# from ntp import NTPClient
from .baseMethod import BaseMethod

SERVER_IP = os.environ["SERVER_IP"]
SERVER_PORT = os.environ["SERVER_PORT"]

HOST = (SERVER_IP, SERVER_PORT)


class NTPClient:
    def __init__(self) -> None:
        self.client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.timeData = {"t0": None, "t1": None, "t2": None, "t3": None}
        self.callBack: function = None

    def startTimeSync(self) -> None:
        self.timeData = {
            "t0": datetime.now().timestamp() * 1000,
            "t1": None,
            "t2": None,
            "t3": None,
        }
        self.sendMes("startSync")
        print("Start time sync: ", self.timeData)

    def recvMes(self) -> dict:
        mes, addr = self.client.recvfrom(1024)
        serverSysTime = int(mes.decode())
        print(f"Receive data: {serverSysTime} from {addr}")
        return self.setTime(serverSysTime)

    def sendMes(self, mes: str) -> None:
        self.client.sendto(mes.encode(), HOST)

    def setTime(self, serverSysTime: int) -> dict:
        self.timeData["t1"] = serverSysTime
        self.timeData["t2"] = serverSysTime
        self.timeData["t3"] = datetime.now().timestamp() * 1000

        t0 = self.timeData["t0"]
        t1 = self.timeData["t1"]
        t2 = self.timeData["t2"]
        t3 = self.timeData["t3"]
        print(f"t0: {t0}, t1: {t1}, t2: {t2}, t3: {t3}")

        delay = round((t3 - t0 - (t2 - t1)) / 2)
        offset = round(((t1 - t0) + (t2 - t3)) / 2)
        os.system(f"sudo date +%s -s @{(t2 + delay) / 1000}")

        print(f"delay: {delay}, offset: {offset}")
        return {"delay": delay, "offset": offset}


# Sync
class Sync(BaseMethod):
    def method(self, payload: NTPClient):
        payload.startTimeSync()
        ntpTime = payload.recvMes()
        delay, offset = ntpTime["delay"], ntpTime["offset"]

        return f"[ sync / Success ] {delay} {offset}"
