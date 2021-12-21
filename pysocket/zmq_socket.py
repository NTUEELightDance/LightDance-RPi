from typing import Union

import zmq


class ZMQSocket:
    def __init__(self, port=8000) -> None:
        context = zmq.Context()
        self.socket = context.socket(zmq.REQ)
        self.socket.connect(f"tcp://localhost:{port}")

    def send(self, message: Union[list, bool, str, int]) -> str:
        if isinstance(message, int) or isinstance(message, bool):
            message = str(message)
        elif isinstance(message, list):
            message = " ".join(message)

        self.socket.send_string(message)

        reply = self.socket.recv()
        return reply.decode()
