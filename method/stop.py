from baseMethod import BaseMethod

# The payload needs to contain a zmq socket client
class Stop(BaseMethod):
    def method(self, payload):
        if not hasattr(payload, "socket"):
            raise KeyError("payload with out socket is not usable")
        socket = payload["socket"]
        socket.send_string("stop")