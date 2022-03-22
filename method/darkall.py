from .baseMethod import BaseMethod


# DarkAll
class DarkAll(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send(["darkall"])
        return response
