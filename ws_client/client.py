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

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")

LED_SAVE_DIR = "./data/LED.json"
OF_SAVE_DIR = "./data/OF.json"

DATA_SAVE_DIR = "/home/pi/LightDance-RPi/data"

class Client:
    def __init__(self):
        super(Client, self).__init__()
        self.url = f"ws://{SERVER_IP}:{SERVER_PORT}"
        self.action = ""
        self.paylaod = {}
        self.ntpclient = NTPClient()

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
            print("execute payload:")
            print(payload)
            status = -1
            message_to_server = ""
            
            try:
                subp = subprocess.Popen(payload, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                outs, errs = subp.communicate()
                outs = outs.decode()
                errs = errs.decode()
                status = subp.poll()
                if status == 0:
                    message_to_server = outs
                    print("Subprocess Success")
                    
                else:
                    message_to_server = errs
                    print("Subprocess Error")
                    print("Error message:")
                    print(errs)
            except:
                print("Unable to run Subprocess")
                message_to_server = "Unable to run Subprocess"
            
            self.parse_response(ws, payload[0], status, message_to_server)

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

            message_to_server = ""
            status = -1

            try:
                subp = subprocess.Popen(["load"])
                message_to_server = "success"
                status = subp.poll()
                print("load complete")
                
            except:
                print("can not run subprocess load")
                message_to_server = "can not run subprocess load"
                status = -1
        
            self.parse_response(ws, action, status, message_to_server)

        elif action == "sync":
            print("sync")

        else:
            print("Invalid action")
            self.parse_response(ws, action, -1, "command not found")

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

    def parse_response(self, ws, command, status, message: str):
        
        ws.send(
            json.dumps(
                {
                    "command":command,
                    "status": status,
                    "payload": {
                        "type": "RPi",
                        "info":{
                            "message": message
                        }
                    }
                }
            )
        )
        print("Send message to server completed")
        print("message:{}".format(message))

    def on_open(self, ws):
        print("Successfully on_open")  # Print Whether successfully on_open
        macaddr = ':'.join(("%012X" % get_mac())[i:i+2] for i in range(0, 12, 2))
        print(macaddr)
        ws.send(
            json.dumps(
                {
                    "command":"boardInfo",
                    "status": 0,
                    "payload": 
                    {
                        "type":"RPi",
                        "info":{
                            "macaddr": macaddr
                        }
                    }
                }
            )
        )
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
