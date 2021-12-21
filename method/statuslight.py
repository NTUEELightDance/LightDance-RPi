from .baseMethod import BaseMethod


# Statuslight
class Statuslight(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("statuslight")
        return response
