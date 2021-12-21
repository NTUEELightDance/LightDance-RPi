from .baseMethod import BaseMethod


# Quit
class Quit(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("quit")
        return response
