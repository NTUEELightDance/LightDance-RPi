# from asyncio.windows_events import NULL
import websocket
import os
import sys
import time

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")
from method import *
from pysocket import ZMQSocket

boardname = "lightdancer-1"  # use"lightdancer-1" to test
cmdlist = [
    "sync",
    "uploadLed",
    "uploadControl",
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


class Client:
    def __init__(self):
        super(Client, self).__init__()
        self.url = "ws://localhost:8082"
        self.cmd = ""
        self.paylaod = {}

        ######zmq#######
        self.socket = ZMQSocket(port=8000)
        self.METHODS = {
            "reboot": Reboot(),
            "boardInfo": BoardInfo(),
            "uploadControl": UploadJsonFile(socket=self.socket),
            "load": Load(socket=self.socket),
            "play": Play(socket=self.socket),
            "pause": Pause(socket=self.socket),
            "stop": Stop(socket=self.socket),
            "statuslight": StatusLight(socket=self.socket),
            "ledtest": LEDTest(socket=self.socket),
            "list": List(socket=self.socket),
            "quit": Quit(socket=self.socket),
            "send": Send(socket=self.socket),
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
            response = self.METHODS[cmd](payload)
            self.parse_response(ws, response)
            print("Send message to rpi complete")
        else:
            print("Failed")

    def ParseServerData(self, message):
        print("Message from server:")
        print(message)
        try:
            self.dictclient = json.loads(message)
            cmd = message["command"]
            payload = None
            if cmd == "play":
                payload = {
                    "start_time": message["payload"]["startTime"],
                    "delay_time": message["payload"]["delay"],
                }
            elif cmd == "lightCurrentStatus":
                payload = None
                # TODO
            elif cmd == "uploadLed":
                payload = None
                # TODO
            elif cmd == "uploadControl":
                payload = None
                # TODO

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

        info = response[-1]
        if command == "boardInfo":
            info = {
                "type": "RPI",
                "dancerName": response[5],
                "ip": response[6],
                "hostName": response[7],
            }
        elif command == "sync":
            # TODO
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
                            "type": "RPI",
                            "dancerName": response[5],
                            "ip": response[6],
                            "hostName": response[7],
                        },
                    },
                }
            )
        )

    def on_close(self, ws):
        print(f"{os.name} closed")

    ####### on_error ########
    # def on_error(self,ws,error):
    #     print("The error is %s" %error)


Test = Client()
Test.startclient()
