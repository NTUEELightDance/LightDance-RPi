from .baseMethod import BaseMethod


# LightAll
class LightAll(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["lightall", payload["color"], payload["alpha"]])
        return response
