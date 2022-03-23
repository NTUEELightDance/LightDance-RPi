# from asyncio.windows_events import NULL
import websocket
import os
import sys
import time

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from method import *
from pysocket import ZMQSocket

cmdlist = [
    "sync",
    "uploadLed",
    "uploadOf",
    "load",
    "play",
    "pause",
    "stop",
    "lightCurrentStatus",
    "kick",
    "shutDown",
    "reboot",
    "boardInfo",
    "init",
    "test",
]
LED_SAVE_DIR = "../data/LED.json"
OF_SAVE_DIR = "../data/OF.json"
HOST = "ws://192.168.10.12:8082"


class Client:
    def __init__(self):
        super(Client, self).__init__()
        self.url = HOST
        self.cmd = ""
        self.paylaod = {}
        self.ntpclient = NTPClient()
        if len(sys.argv) != 2:
            print("Usage: python3 client/client.py <dancerName>")
            exit()
        self.dancerName = sys.argv[1]

        ######zmq#######
        self.socket = ZMQSocket(port=8000)
        self.METHODS = {
            "reboot": Reboot(),
            "boardInfo": BoardInfo(),
            "uploadOf": UploadJsonFile(socket=self.socket),
            "load": Load(socket=self.socket),
            "play": Play(socket=self.socket),
            "pause": Pause(socket=self.socket),
            "stop": Stop(socket=self.socket),
            "statuslight": StatusLight(socket=self.socket),
            "ledtest": LEDTest(socket=self.socket),
            "list": List(socket=self.socket),
            "quit": Quit(socket=self.socket),
            "send": Send(socket=self.socket),
            "uploadLed": UploadJsonFile(socket=self.socket),
            "sync": Sync(socket=self.socket),
            "shutDown": ShutDown(socket=self.socket),
        }
        ##############

    def startclient(self):
        while True:
            try:
                ws = websocket.WebSocketApp(
                    self.url,
                    on_message=self.on_message,
                    on_open=self.on_open,
                    on_close=self.on_close,
                )
                ws.run_forever()
                time.sleep(3)
            except websocket.WebSocketException:
                print("Failed to connect")

    def on_message(self, ws, message):
        cmd, payload = self.ParseServerData(message)
        if self.Check(ws, cmd, payload):
            if cmd != "sync":
                response = self.METHODS[cmd](payload)
            else:
                response = self.METHODS[cmd](self.ntpclient)
            self.parse_response(ws, response)
            print("Send message to rpi complete")
        else:
            print("Failed")

    def ParseServerData(self, message):
        print("Message from server:")
        print(message)
        try:
            message = json.loads(message)
            cmd = message["command"]
            payload = None
            if cmd == "play":
                payload = {
                    "start_time": message["payload"]["startTime"],
                    "delay_time": message["payload"]["delay"],
                }
                # TODO: calculate message["payload"]["sysTime"]
            elif cmd == "lightCurrentStatus":
                payload = None
                # TODO
            elif cmd == "uploadLed":
                payload = {"file": message["payload"], "dir": LED_SAVE_DIR}
            elif cmd == "uploadOf":
                payload = {"file": message["payload"], "dir": OF_SAVE_DIR}
            elif cmd == "load":
                payload = {"path": "./data/"}

            return cmd, payload
        except:
            print("Invalid json format:")
            print(message)

    def Check(self, ws, cmd, payload):
        if cmd in cmdlist:
            return True
        print(f"{cmd} not found in cmdlist: {cmdlist}")
        return False

    def parse_response(self, ws, response: str):
        print(response)  # print the response
        response = response.split(" ")
        command = response[1]
        status = response[3]

        info = " ".join(response[5:]) if len(response) > 5 else "good!!!"
        if command == "boardInfo":
            info = {
                "type": "RPi",
                "dancerName": response[5],
                "ip": response[6],
                "hostName": response[7],
            }
        elif command == "sync":
            delay, offset = response[5], response[6]
            info = {"delay": int(delay), "offset": int(offset)}
            pass

        ws.send(
            json.dumps(
                {
                    "command": command,
                    "payload": {"success": status == "Success", "info": info},
                }
            )
        )

    def on_open(self, ws):
        print("Successfully on_open")  # Print Whether successfully on_open
        response = self.METHODS["boardInfo"]()
        response = response.split(" ")
        ws.send(
            json.dumps(
                {
                    "command": "boardInfo",
                    "payload": {
                        "success": response[3] == "Success",
                        "info": {
                            "type": "RPi",
                            "dancerName": self.dancerName,
                            "ip": response[5],
                            "hostName": response[6],
                        },
                    },
                }
            )
        )

    def on_close(self, ws):
        print(f"{self.dancerName} closed")

    ####### on_error ########
    # def on_error(self,ws,error):
    #     print("The error is %s" %error)


if __name__ == "__main__":
    Test = Client()
    Test.startclient()
