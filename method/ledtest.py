from .baseMethod import BaseMethod


# Ledtest
class Ledtest(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("ledtest")
        return response
