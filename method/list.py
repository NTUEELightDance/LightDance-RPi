from .baseMethod import BaseMethod


# List
class List(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("list")
        return response
