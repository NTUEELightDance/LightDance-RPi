from .baseMethod import BaseMethod


class Blue(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["lightall", int("0x0000ff", -1), 10])
        return response
