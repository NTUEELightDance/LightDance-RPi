#!/Users/ray/opt/miniconda3/bin/python3

import asyncio
import json
import os
from string import Template

import websockets
from config import *


def get_json(topic, statusCode, payload):
    return {
        "from": "server",
        "topic": topic,
        "statusCode": statusCode,
        "payload": payload,
    }
    
def parse_client_response(response):
    response = json.loads(response)
    topic = response["topic"]
    statusCode = response["statusCode"]
    payload = response["payload"]
    print(f"Topic: {topic}")
    print(f"Status code: {statusCode}")
    print(f"Payload: {payload}")
    return topic, statusCode, payload

async def handler(websocket):
    mode = ""
    info = await websocket.recv()
    print(info)
    while True:
        if mode == "command":
            command = input("Enter command: ")
            if command == "exit":
                mode = ""
            else:
                message = get_json("command", 0, command)
                print(message)
                await websocket.send(json.dumps(message))
                response = await websocket.recv()
                topic, statusCode, payload = parse_client_response(response)
                
                command = payload["command"]
                result = payload["message"]
                print(f"Command: {command}")
                print(f"Result: {result}")

        elif mode == "upload":
            control_json = json.loads(open("data/control.json", "r").read())
            of_json = json.loads(open("data/OF.json", "r").read())
            led_json = json.loads(open("data/LED.json", "r").read())
            message = get_json("upload", 0, [control_json, of_json, led_json])
            print(message)
            await websocket.send(json.dumps(message))
            mode = ""

        # elif mode == "sync":
        #     if command == "exit":
        #         mode = ""
        #     else:
        #         command_json["action"] = "sync"
        #         command_json["payload"] = []
        #         print(command_json)
        #         await websocket.send(json.dumps(command_json))

        else:
            mode = input("Enter mode: ")


async def main():
    async with websockets.serve(handler, SERVER_IP, SERVER_PORT):
        await asyncio.Future()  # run forever


if __name__ == "__main__":
    asyncio.run(main())
