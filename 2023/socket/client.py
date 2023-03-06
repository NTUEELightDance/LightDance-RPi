# from asyncio.windows_events import NULL
import json
import os
import sys
import time
import subprocess

import websocket

from ntpclient import *
# from boardInfo import *

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")

cmdlist = [
    "command",
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
    "red",
    "green",
    "blue",
    "darkall",
    "stmInit",
    "restartController",
]
LED_SAVE_DIR = "./data/LED.json"
OF_SAVE_DIR = "./data/OF.json"


SERVER_IP = "0.0.0.0"
SERVER_PORT = "8082"


class Client:
    def __init__(self):
        super(Client, self).__init__()
        self.url = f"ws://{SERVER_IP}:{SERVER_PORT}"
        self.action = ""
        self.paylaod = {}
        self.ntpclient = NTPClient()
        # if len(sys.argv) != 2:
        #     print("Usage: python3 client/client.py <dancerName>")
        #     exit()
        # self.dancerName = sys.argv[1]
        self.dancerName = "Arthur"

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
        action, payload = self.ParseServerData(message)
        
        if action == "command":
            if self.Check(ws, action, payload):
                print("execute payload:")
                print(payload)
                subp = subprocess.Popen(payload)
                print(subp)
                subp.wait(2)
                if subp.poll() == 0:
                    print("Subprocess Success")
                else:
                    print("Subprocess Failed")
            else:
                print("Failed")
        
        elif action == "upload":
            with open("payload.json", "w") as f:
                json.dump(payload, f, indent = 4)
        
        elif action == "sync":
            print("sync")

        else:
            print("Invalid action")

        # self.parse_response(ws, response)
        print("Send message to rpi complete")
        

    def ParseServerData(self, message):
        print("Message from server:")
        print(message)
        try:
            message = json.loads(message)
            action = message["action"]
            payload = message["payload"]
            print(action, payload)
            return action, payload
        except:
            print("Invalid json format:")
            print(message)

    def Check(self, ws, action, payload):
        if action in cmdlist:
            return True
        print(f"{action} not found in cmdlist: {cmdlist}")
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
        # response = self.METHODS["boardInfo"]()
        # print(response)
        # response = response.split(" ")
        ws.send(
            json.dumps(
                {
                    "command": "boardInfo",
                    "payload": {
                        "success": True,
                        "info": {
                            "type": "RPi",
                            "dancerName": self.dancerName,
                            "ip": "127.0.0.1",
                            "hostName": "hostname",
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
    print("Start")
    Test = Client()
    Test.startclient()
