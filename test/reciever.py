import time

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
    socket.send_string(f"Success: {message.decode()}")
