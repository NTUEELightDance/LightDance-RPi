from .baseMethod import BaseMethod


# sendLight
class sendLight(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["sendlight", payload["id"], payload["vector"]])
        return response
