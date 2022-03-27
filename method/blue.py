from .baseMethod import BaseMethod


class Blue(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["lightall", str(int("0000ff", 16)), str(10)])
        return response
