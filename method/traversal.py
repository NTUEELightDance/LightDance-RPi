from .baseMethod import BaseMethod


# Traversal
class Traversal(BaseMethod):
    def method(self, payload=None):
        response = self.socket.send("traversal")
        return response
