from .baseMethod import BaseMethod


# Load
class Load(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["load", payload["path"]])
        return response
