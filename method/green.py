from .baseMethod import BaseMethod


class Green(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["lightall", str(int("00ff00", 16)), str(10)])
        return response
