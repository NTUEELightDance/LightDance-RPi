# from asyncio.windows_events import NULL
import json
import os
import sys
import time
import subprocess

import websocket
from getmac import get_mac_address

from config import *
# from ntpclient import *

sys.path.append(os.path.dirname(os.path.abspath(__file__)) + "/..")

DATA_SAVE_DIR = "/home/pi/LightDance-RPi/data"


class Client:
    def __init__(self):
        super(Client, self).__init__()
        self.url = f"ws://{SERVER_IP}:{SERVER_PORT}"
        self.action = ""
        self.payload = {}
        # self.ntp_client = NTPClient()
        self.MAC = ""
        self.ws = None

    def start_client(self):
        print(f"Connecting to {self.url}")
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
        print(f"[To Server] {message}")
        self.ws.send(message)

    def on_open(self, ws):
        print("Successfully on_open")
        self.MAC = get_mac_address()
        print(f"[Info] MAC address: {self.MAC}")
        self.send_response("boardInfo", 0, {"MAC": self.MAC})
        # print("Mac address sent")

    def on_close(self, ws, close_status_code, close_msg):
        print(f"websocket closed")

    def on_error(self, ws, error):
        print("[Error] %s" % error)

    def parse_server_data(self, message):
        try:
            message = json.loads(message)
            topic = message["topic"]
            payload = message["payload"]
            payload_short = (
                str(payload)[:200] + "..." if len(str(payload)) > 200 else str(payload)
            )
            print(f"[From Server] Topic: {topic}, Payload: {payload_short}")
            return topic, payload
        except:
            print("Invalid json format:")
            print(message)

    def on_message(self, ws, message):
        print()
        # self.MAC = get_mac_address()
        topic, payload = self.parse_server_data(message)

        if topic == "command":
            # print("execute payload:")
            # print(payload)
            status = -1
            message_to_server = ""

            try:
                # print(f"Executing command: {payload}")
                process = subprocess.Popen(
                    payload, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True
                )
                outs, errs = process.communicate()
                outs = outs.decode()
                errs = errs.decode()
                status = process.poll()
                if status == 0:
                    message_to_server = outs
                    print("[Info] Subprocess Success")

                else:
                    message_to_server = errs
                    print(f'[Error] Subprocess Error: "{errs}"')
            except:
                print("Unable to run Subprocess")
                message_to_server = "Unable to run Subprocess"

            payload_to_server = {
                "MAC": self.MAC,
                "command": payload,
                "message": message_to_server,
            }
            # print(payload_to_server)
            self.send_response("command", status, payload_to_server)

        elif topic == "upload":
            with open(os.path.join(DATA_SAVE_DIR, "control.json"), "w") as f:
                print("[Info] Writing control.json")
                json.dump(payload[0], f, indent=4)
            with open(os.path.join(DATA_SAVE_DIR, "OF.json"), "w") as f:
                print("[Info] Writing OF.json")
                json.dump(payload[1], f, indent=4)
            with open(os.path.join(DATA_SAVE_DIR, "LED.json"), "w") as f:
                print("[Info] Writing LED.json")
                json.dump(payload[2], f, indent=4)

            status = 0
            message_to_server = ""
            with open(os.path.join(DATA_SAVE_DIR, "control.json"), "r") as f:
                print("[Info] Checking control.json")
                control = json.load(f)
                if control != payload[0]:
                    status -= 1
                    message_to_server += "control.json not match. "

            with open(os.path.join(DATA_SAVE_DIR, "OF.json"), "r") as f:
                print("[Info] Checking OF.json")
                OF = json.load(f)
                if OF != payload[1]:
                    status -= 1
                    message_to_server += "OF.json not match. "

            with open(os.path.join(DATA_SAVE_DIR, "LED.json"), "r") as f:
                print("[Info] Checking LED.json")
                LED = json.load(f)
                if LED != payload[2]:
                    status -= 1
                    message_to_server += "LED.json not match."

            if status == 0:
                print("[Info] Upload Success")
                message_to_server = "Success"

            self.send_response(
                "upload",
                status,
                {
                    "MAC": self.MAC,
                    "command": "upload",
                    "message": message_to_server,
                },
            )

        elif topic == "ping":
            self.send_response(
                "ping",
                0,
                {
                    "MAC": self.MAC,
                    "command": "ping",
                    "message": "pong",
                },
            )

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
