from .baseMethod import BaseMethod


class Green(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["lightall", int('0x00ff00', -1), 10])
        return response

