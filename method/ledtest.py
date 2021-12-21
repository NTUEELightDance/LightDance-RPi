from .baseMethod import BaseMethod


# Statuslight
class Ledtest(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("ledtest")
        return response
