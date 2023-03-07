#!/Users/ray/opt/miniconda3/bin/python3

import asyncio
import json
import os
from string import Template

import websockets

command_json = dict()

SERVER_IP = os.environ["SERVER_IP"]
SERVER_PORT = int(os.environ["SERVER_PORT"])


async def handler(websocket):
    mode = ""
    info = await websocket.recv()
    while True:
        if mode == "command":
            command = input("Enter command: ")
            if command == "exit":
                mode = ""
            else:
                command_json["action"] = "command"
                command_json["payload"] = command.split()
                print(command_json)
                await websocket.send(json.dumps(command_json))

        elif mode == "upload":
            command_json["action"] = "upload"
            control_json = json.loads(open("data/control.json", "r").read())
            of_json = json.loads(open("data/OF.json", "r").read())
            led_json = json.loads(open("data/LED.json", "r").read())
            command_json["payload"] = [control_json, of_json, led_json]
            print(command_json)
            await websocket.send(json.dumps(command_json))
            mode = ""

        elif mode == "sync":
            if command == "exit":
                mode = ""
            else:
                command_json["action"] = "sync"
                command_json["payload"] = []
                print(command_json)
                await websocket.send(json.dumps(command_json))

        else:
            mode = input("Enter mode: ")


async def main():
    async with websockets.serve(handler, SERVER_IP, SERVER_PORT):
        await asyncio.Future()  # run forever


if __name__ == "__main__":
    asyncio.run(main())
