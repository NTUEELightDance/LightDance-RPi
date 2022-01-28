import random
import time
from logging import warning

import zmq

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:8000")

while True:
    #  Wait for next request from client
    message = socket.recv()
    print("Received request: %s" % message.decode())

    # time.sleep(1)

    #  Send reply back to client
    status = random.choice(["Success", "Error", "Warning"])
    socket.send_string(f"{status}: {message.decode()}")
