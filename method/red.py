from .baseMethod import BaseMethod


class Red(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["lightall", int('0xff0000', -1), 10])
        return response

