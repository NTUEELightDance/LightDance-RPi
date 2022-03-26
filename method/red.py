from .baseMethod import BaseMethod


class Red(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["lightall", str(int("ff0000", 16)), str(10)])
        return response
