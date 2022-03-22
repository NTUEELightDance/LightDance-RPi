from .baseMethod import BaseMethod


# OFtest
class OFTest(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(
            ["oftest", payload["channel"], payload["color"], payload["alpha"]]
        )
        return response
