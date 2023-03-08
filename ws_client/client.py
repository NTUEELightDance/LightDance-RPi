#!/usr/bin/env python3

# from asyncio.windows_events import NULL
import json
import os
import sys
import time
import subprocess

import websocket
from uuid import getnode as get_mac

from config import *
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

DATA_SAVE_DIR = "/tmp/lightdance"

# SERVER_IP = os.environ["SERVER_IP"]
# SERVER_PORT = int(os.environ["SERVER_PORT"])


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
        # self.dancerName = "Arthur"
        # self.dancerName = os.environ["DANCER_NAME"]

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
                subp = subprocess.Popen(payload, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                outs, errs = subp.communicate()
                outs = outs.decode()
                errs = errs.decode()
                print(outs)
                if subp.poll() == 0:
                    print("Subprocess Success")
                    
                else:
                    print("Subprocess Failed")
                    print("Error message:")
                    print(errs)
                self.parse_response(ws, subp.poll(), outs, errs)

            else:
                print("ERROR: Command not in command list")

        elif action == "upload":
            print("upload")
            try:
                os.mkdir(DATA_SAVE_DIR)
            except:
                print("dirctory /lightdance exist")

            print(os.path.join(DATA_SAVE_DIR, "control.json"))
            with open(os.path.join(DATA_SAVE_DIR, "control.json"), "w") as f:
                print("Writing control.json")
                json.dump(payload[0], f, indent=4)
            with open(os.path.join(DATA_SAVE_DIR, "OF.json"), "w") as f:
                print("Writing OF.json")
                json.dump(payload[1], f, indent=4)
            with open(os.path.join(DATA_SAVE_DIR, "LED.json"), "w") as f:
                print("Writing LED.json")
                json.dump(payload[2], f, indent=4)

            subp = subprocess.Popen(["load"])
            # subp = subprocess.Popen(["load", "dancer", os.path.join(DATA_SAVE_DIR, "control.json")])
            # subp = subprocess.Popen(["load", "OF", os.path.join(DATA_SAVE_DIR, "OF.json")])
            # subp = subprocess.Popen(["load", "LED", os.path.join(DATA_SAVE_DIR, "LED.json")])
            print("load complete")

            self.parse_response(ws, 0, "success", "success")

        elif action == "sync":
            print("sync")

        else:
            print("Invalid action")
            self.parse_response(ws, -1, "success", "command not found")

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

    def parse_response(self, ws, status, outs: str, errs: str):
        print(outs)  # print the response
        response = outs
        if status != 0:
            response = errs

        ws.send(
            json.dumps(
                {
                    "status": status,
                    "payload": response
                }
            )
        )
        print("Send response complete")

    def on_open(self, ws):
        print("Successfully on_open")  # Print Whether successfully on_open
        macaddr = ':'.join(("%012X" % get_mac())[i:i+2] for i in range(0, 12, 2))
        print(macaddr)
        ws.send(
            json.dumps(
                {
                    "status": 0,
                    "payload": macaddr
                }
            )
        )
        # ws.send("hello")
        print("Mac address sent")

    def on_close(self, ws):
        print("websocket closed")

    ####### on_error ########
    # def on_error(self,ws,error):
    #     print("The error is %s" %error)


if __name__ == "__main__":
    print("Start")
    Test = Client()
    Test.startclient()
