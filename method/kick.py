from .baseMethod import BaseMethod


# Kick is only for socket usage, cli and wire won't need this function
class Kick(BaseMethod):
    def method(self, payload):
        if not hasattr(payload, "socket"):
            raise KeyError("payload without socket is not usable")
        if not hasattr(payload, "url"):
            raise KeyError("payload without url is not usable")
        socket = payload["socket"]
        url = payload["url"]
        socket.close()

        context = zmq.Context()
        socket = context.socket(zmq.REQ)
        socket.connect(url)

        # TODO:same action with pause
        return socket
