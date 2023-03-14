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

DATA_SAVE_DIR = "/Users/ray/Desktop/lightdance/LightDance-RPi-ray/data"


class Client:
    def __init__(self):
        super(Client, self).__init__()
        self.url = f"ws://{SERVER_IP}:{SERVER_PORT}"
        self.action = ""
        self.payload = {}
        self.ntp_client = NTPClient()
        self.MAC = ":".join(("%012X" % get_mac())[i : i + 2] for i in range(0, 12, 2))
        self.ws = None

    def start_client(self):
        print(f"connecting to {self.url}")
        while True:
            try:
                self.ws = websocket.WebSocketApp(
                    self.url,
                    on_open=self.on_open,
                    on_message=self.on_message,
                    on_close=self.on_close,
                    on_error=self.on_error,
                )
                self.ws.run_forever()
                time.sleep(3)
            except websocket.WebSocketException:
                print("Failed to connect")

    def send_response(self, topic: str, statusCode: int, payload: dict):
        message = json.dumps(
            {
                "from": "RPi",
                "topic": topic,
                "statusCode": statusCode,
                "payload": payload,
            }
        )
        print("Send message to server:")
        print(message)
        self.ws.send(message)

    def on_open(self, ws):
        print("Successfully on_open")
        print(f"MAC address: {self.MAC}")
        self.send_response("boardInfo", 0, {"MAC": self.MAC})
        print("Mac address sent")

    def on_close(self, ws):
        print("websocket closed")

    def on_error(self, ws, error):
        print("The error is %s" % error)

    def parse_server_data(self, message):
        print("Message from server:")
        print(message)
        try:
            message = json.loads(message)
            topic = message["topic"]
            payload = message["payload"]
            print(topic, payload)
            return topic, payload
        except:
            print("Invalid json format:")
            print(message)

    def on_message(self, ws, message):
        topic, payload = self.parse_server_data(message)

        if topic == "command":
            print("execute payload:")
            print(payload)
            status = -1
            message_to_server = ""

            try:
                process = subprocess.Popen(
                    payload, stdout=subprocess.PIPE, stderr=subprocess.PIPE
                )
                outs, errs = process.communicate()
                outs = outs.decode()
                errs = errs.decode()
                status = process.poll()
                if status == 0:
                    message_to_server = outs
                    print("Subprocess Success")

                else:
                    message_to_server = errs
                    print("Subprocess Error")
                    print("Error message:")
                    print(errs)
            except:
                print("hi")
                print("Unable to run Subprocess")
                message_to_server = "Unable to run Subprocess"

            command = [str(w) for w in payload]
            payload_to_server = {
                "MAC": self.MAC,
                "command": " ".join(command),
                "message": message_to_server,
            }
            print(payload_to_server)
            self.send_response("command", status, payload_to_server)

        elif topic == "upload":
            print("upload")
            try:
                os.mkdir(DATA_SAVE_DIR)
            except:
                print(f"directory {DATA_SAVE_DIR} exist")

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
                process = subprocess.Popen(["load"])
                message_to_server = "success"
                status = process.poll()
                print("load complete")

            except:
                print("Can't run subprocess load")
                message_to_server = "can not run subprocess load"
                status = -1

            self.send_response(
                "command",
                status,
                {
                    "MAC": self.MAC,
                    "command": "load",
                    "message": message_to_server,
                },
            )

        elif topic == "sync":
            print("sync")

        else:
            print("Invalid action")
            self.send_response(
                topic,
                -1,
                {
                    "MAC": self.MAC,
                    "command": " ".join(payload),
                    "message": "Invalid action",
                },
            )


if __name__ == "__main__":
    Test = Client()
    Test.start_client()
