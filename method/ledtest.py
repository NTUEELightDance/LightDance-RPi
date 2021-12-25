from .baseMethod import BaseMethod


# Ledtest
class LEDTest(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("ledtest")
        return response
