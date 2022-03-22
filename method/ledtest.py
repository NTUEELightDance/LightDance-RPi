from .baseMethod import BaseMethod


# LEDtest
class LEDTest(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(
            ["ledtest", payload["channel"], payload["color"], payload["alpha"]]
        )
        return response
