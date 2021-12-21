from .baseMethod import BaseMethod


# Pause
class Pause(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("pause")
        return response
