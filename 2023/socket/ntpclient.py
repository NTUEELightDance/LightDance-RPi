import os
import socket
from datetime import datetime

HOST = ("192.168.0.200", 7122)


class NTPClient:
    def __init__(self) -> None:
        self.client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.timeData = {"t0": None, "t1": None, "t2": None, "t3": None}
        self.callBack: function = None

    def startTimeSync(self) -> None:
        self.timeData = {
            "t0": datetime.now().timestamp(),
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
        self.timeData["t3"] = datetime.now().timestamp()

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
